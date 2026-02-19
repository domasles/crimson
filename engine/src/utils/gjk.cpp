#include <pch.h>

#include <utils/gjk.h>

namespace engine::utils::gjk {
    static float dot(const Vector2& a, const Vector2& b) {
        return a.getRawX() * b.getRawX() + a.getRawY() * b.getRawY();
    }

    static Vector2 neg(const Vector2& v) {
        return Vector2{-v.getRawX(), -v.getRawY()};
    }

    static Vector2 perp(const Vector2& v, const Vector2& toward) {
        Vector2 p{-v.getRawY(), v.getRawX()};
        return dot(p, toward) >= 0.0f ? p : neg(p);
    }

    static Vector2 supportPoint(const CollisionShape& shapeA, const Vector2& posA, const Vector2& sizeA, const CollisionShape& shapeB, const Vector2& posB, const Vector2& sizeB, const Vector2& dir) {
        return shapeA.getSupportPoint(posA, sizeA, dir) - shapeB.getSupportPoint(posB, sizeB, neg(dir));
    }

    struct Simplex {
        std::array<Vector2, 3> pts;

        int size = 0;
        void push(const Vector2& v) { pts[size++] = v; }
    };

    static bool doLineSimplex(Simplex& s, Vector2& d) {
        Vector2 A = s.pts[1];
        Vector2 B = s.pts[0];
        Vector2 AB = B - A;
        Vector2 AO = neg(A);

        if (dot(AB, AO) > 0.0f) d = perp(AB, AO);

        else {
            s.pts[0] = A;
            s.size = 1;
            d = AO;
        }

        return false;
    }

    static bool doTriangleSimplex(Simplex& s, Vector2& d) {
        Vector2 A = s.pts[2];
        Vector2 B = s.pts[1];
        Vector2 C = s.pts[0];
        Vector2 AB = B - A;
        Vector2 AC = C - A;
        Vector2 AO = neg(A);

        Vector2 ABperp = perp(AB, neg(AC));
        Vector2 ACperp = perp(AC, neg(AB));

        if (dot(ABperp, AO) > 0.0f) {
            s.pts[0] = B;
            s.pts[1] = A;
            s.size = 2;

            d = ABperp;

            return false;
        }

        if (dot(ACperp, AO) > 0.0f) {
            s.pts[1] = A;
            s.size = 2;

            d = ACperp;

            return false;
        }

        return true;
    }

    static bool doSimplex(Simplex& s, Vector2& d) {
        if (s.size == 2) return doLineSimplex(s, d);
        return doTriangleSimplex(s, d);
    }

    struct EdgeInfo {
        Vector2 normal;

        float dist = 0.0f;
        int next = 0;
    };

    static EdgeInfo closestEdge(const Vector2* poly, int size) {
        EdgeInfo best;
        best.dist = 1e38f;

        for (int i = 0; i < size; i++) {
            int j = (i + 1) % size;
            Vector2 edge = poly[j] - poly[i];

            float len = std::sqrt(dot(edge, edge));
            if (len < 1e-10f) continue;

            Vector2 n{edge.getRawY() / len, -edge.getRawX() / len};
            float d = dot(n, poly[i]);

            if (d < 0.0f) {
                n = neg(n);
                d = -d;
            }

            if (d < best.dist) {
                best.dist = d;
                best.normal = n;
                best.next = j;
            }
        }

        return best;
    }

    static std::pair<Vector2, float> epa(const Simplex& gjkSimplex, const CollisionShape& shapeA, const Vector2& posA, const Vector2& sizeA, const CollisionShape& shapeB, const Vector2& posB, const Vector2& sizeB) {
        constexpr int MAX_POLY = 32;
        std::array<Vector2, MAX_POLY> poly;
        int polySize = gjkSimplex.size;

        for (int i = 0; i < polySize; i++) poly[i] = gjkSimplex.pts[i];

        for (int iter = 0; iter < 64; iter++) {
            if (polySize >= MAX_POLY) break;

            EdgeInfo edge = closestEdge(poly.data(), polySize);
            Vector2 sup = supportPoint(shapeA, posA, sizeA, shapeB, posB, sizeB, edge.normal);

            float d = dot(edge.normal, sup);
            if (d - edge.dist < 0.001f) return {edge.normal, edge.dist + 0.001f};

            for (int i = polySize; i > edge.next; --i) poly[i] = poly[i - 1];
            poly[edge.next] = sup;
            polySize++;
        }

        EdgeInfo edge = closestEdge(poly.data(), polySize);
        return {edge.normal, edge.dist + 0.001f};
    }

    CollisionResult test(const CollisionShape& shapeA, const Vector2& posA, const Vector2& sizeA, const CollisionShape& shapeB, const Vector2& posB, const Vector2& sizeB) {
        CollisionResult result;

        Vector2 cA = posA + sizeA * 0.5f;
        Vector2 cB = posB + sizeB * 0.5f;
        Vector2 d = cB - cA;

        if (dot(d, d) < 1e-10f) d = Vector2{1.0f, 0.0f};

        Simplex simplex;

        simplex.push(supportPoint(shapeA, posA, sizeA, shapeB, posB, sizeB, d));
        d = neg(simplex.pts[0]);

        if (dot(d, d) < 1e-10f) d = Vector2{1.0f, 0.0f};

        for (int iter = 0; iter < 64; iter++) {
            Vector2 a = supportPoint(shapeA, posA, sizeA, shapeB, posB, sizeB, d);

            if (dot(a, d) < 0.0f) return result;

            simplex.push(a);

            if (doSimplex(simplex, d)) {
                result.hasCollision = true;

                if (simplex.size < 3) {
                    result.contactNormal = neg(d.normalize());
                    result.contactPoint = shapeA.getSupportPoint(posA, sizeA, d.normalize());
                }

                else {
                    auto [normal, depth] = epa(simplex, shapeA, posA, sizeA, shapeB, posB, sizeB);

                    result.contactNormal = neg(normal);
                    result.contactPoint = shapeA.getSupportPoint(posA, sizeA, normal);
                }

                return result;
            }
        }

        return result;
    }
}

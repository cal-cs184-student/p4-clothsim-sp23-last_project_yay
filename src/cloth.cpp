#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
    this->width = width;
    this->height = height;
    this->num_width_points = num_width_points;
    this->num_height_points = num_height_points;
    this->thickness = thickness;

    buildGrid();
    buildClothMesh();
}

Cloth::~Cloth() {
    point_masses.clear();
    springs.clear();

    if (clothMesh) {
        delete clothMesh;
    }
}

void Cloth::buildGrid() {
    double rowStep = height / num_height_points;
    double colStep = width / num_width_points;

    auto pmRefIndex = [this] (size_t row, size_t col) {
        return &point_masses[row * num_width_points + col];
    };

    auto makeConstraints = [this, pmRefIndex] (size_t row, size_t col) {
        PointMass *c = pmRefIndex(row, col);
        if (col != 0)
            springs.emplace_back(c, pmRefIndex(row, col-1), STRUCTURAL);

        if (row != 0)
            springs.emplace_back(c, pmRefIndex(row-1, col), STRUCTURAL);

        if (col != 0 && row != 0)
            springs.emplace_back(c, pmRefIndex(row-1, col-1), SHEARING);

        if (col != num_width_points - 1 && row != 0)
            springs.emplace_back(c, pmRefIndex(row-1, col+1), SHEARING);

        if (col > 1)
            springs.emplace_back(c, pmRefIndex(row, col-2), BENDING);

        if (row > 1)
            springs.emplace_back(c, pmRefIndex(row-2, col), BENDING);
    };

    point_masses.reserve(num_width_points * num_height_points);
    springs.reserve(num_width_points * num_height_points * 6);
    if (orientation == HORIZONTAL) {
        for (int row = 0; row < num_height_points; row++) {
            for (int col = 0; col < num_width_points; col++) {
                Vector3D pos(colStep * col, 1., rowStep * row);
                point_masses.emplace_back(pos, false);
                makeConstraints(row, col);
            }
        }
    } else {
        for (int row = 0; row < num_height_points; row++) {
            for (int col = 0; col < num_width_points; col++) {
                double offset = (rand() * 2.0 / RAND_MAX - 1) / 1000.0;
                Vector3D pos(colStep * col, rowStep * row, offset);
                point_masses.emplace_back(pos, false);
                makeConstraints(row, col);
            }
        }
    }

    for (vector<int> xy : pinned) {
        pmRefIndex(xy[0], xy[1])->pinned = true;
    }
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
    double mass = width * height * cp->density / num_width_points / num_height_points;
    double delta_t = 1.0f / frames_per_sec / simulation_steps;

    // TODO (Part 2): Compute total force acting on each point mass.

    Vector3D forces(0.);
    for (Vector3D acceleration : external_accelerations) {
        forces += acceleration;
    }
    forces *= mass;

    for (size_t i = 0; i < point_masses.size(); i++)
        point_masses[i].forces = forces;

    for (Spring sp : springs) {
        e_spring_type type = sp.spring_type;
        if (!cp->enable_bending_constraints && type == BENDING)
            continue;
        if (!cp->enable_shearing_constraints && type == SHEARING)
            continue;
        if (!cp->enable_structural_constraints && type == STRUCTURAL)
            continue;

        double k = type==BENDING ? cp->ks * .2 : cp->ks;
        Vector3D posDelta = sp.pm_a->position - sp.pm_b->position;
        forces = k * (posDelta.norm() - sp.rest_length) * posDelta.unit();

        // PosDelta is pm_b->pm_a
        sp.pm_a->forces -= forces;
        sp.pm_b->forces += forces;
    }

    // TODO (Part 2): Use Verlet integration to compute new point mass positions

    for (size_t i = 0; i < point_masses.size(); i++) {
        const PointMass pm = point_masses[i];
        if (pm.pinned)
            continue;
        Vector3D a = pm.forces / mass;
        Vector3D last_position = pm.last_position;
        point_masses[i].last_position = pm.position;
        point_masses[i].position += (1 - cp->damping / 100) * (pm.position - last_position) + a * pow(delta_t, 2);
    }

    // TODO (Part 4): Handle self-collisions.

    // TODO (Part 3): Handle collisions with other primitives.

    for (CollisionObject *obj : *collision_objects)
        for (PointMass &pm : point_masses)
            obj->collide(pm);

    // TODO (Part 2): Constrain the changes to be such that the spring does not change
    // in length more than 10% per timestep [Provot 1995].

    for (size_t i = 0; i < springs.size(); i++) {
        const Spring sp = springs[i];
        Vector3D posDelta = sp.pm_a->position - sp.pm_b->position;
        double correctionLength = posDelta.norm() - 1.1 * sp.rest_length;
        if (correctionLength <= 0)
            continue;

        Vector3D correction = posDelta.unit() * correctionLength;
        if (sp.pm_b->pinned) {
            sp.pm_a->position -= correction;
        } else if (sp.pm_a->pinned) {
            sp.pm_b->position += correction;
        } else {
            sp.pm_a->position -= correction * .5;
            sp.pm_b->position += correction * .5;
        }
    }
}

void Cloth::build_spatial_map() {
    for (const auto &entry : map) {
        delete(entry.second);
    }
    map.clear();

    // TODO (Part 4): Build a spatial map out of all of the point masses.

}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
    // TODO (Part 4): Handle self-collision for a given point mass.

}

float Cloth::hash_position(Vector3D pos) {
    // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.

    return 0.f;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
    PointMass *pm = &point_masses[0];
    for (int i = 0; i < point_masses.size(); i++) {
        pm->position = pm->start_position;
        pm->last_position = pm->start_position;
        pm++;
    }
}

void Cloth::buildClothMesh() {
    if (point_masses.size() == 0) return;

    ClothMesh *clothMesh = new ClothMesh();
    vector<Triangle *> triangles;

    // Create vector of triangles
    for (int y = 0; y < num_height_points - 1; y++) {
        for (int x = 0; x < num_width_points - 1; x++) {
            PointMass *pm = &point_masses[y * num_width_points + x];
            // Get neighboring point masses:
            /*                      *
             * pm_A -------- pm_B   *
             *             /        *
             *  |         /   |     *
             *  |        /    |     *
             *  |       /     |     *
             *  |      /      |     *
             *  |     /       |     *
             *  |    /        |     *
             *      /               *
             * pm_C -------- pm_D   *
             *                      *
             */

            float u_min = x;
            u_min /= num_width_points - 1;
            float u_max = x + 1;
            u_max /= num_width_points - 1;
            float v_min = y;
            v_min /= num_height_points - 1;
            float v_max = y + 1;
            v_max /= num_height_points - 1;

            PointMass *pm_A = pm                       ;
            PointMass *pm_B = pm                    + 1;
            PointMass *pm_C = pm + num_width_points    ;
            PointMass *pm_D = pm + num_width_points + 1;

            Vector3D uv_A = Vector3D(u_min, v_min, 0);
            Vector3D uv_B = Vector3D(u_max, v_min, 0);
            Vector3D uv_C = Vector3D(u_min, v_max, 0);
            Vector3D uv_D = Vector3D(u_max, v_max, 0);


            // Both triangles defined by vertices in counter-clockwise orientation
            triangles.push_back(new Triangle(pm_A, pm_C, pm_B,
                                             uv_A, uv_C, uv_B));
            triangles.push_back(new Triangle(pm_B, pm_C, pm_D,
                                             uv_B, uv_C, uv_D));
        }
    }

    // For each triangle in row-order, create 3 edges and 3 internal halfedges
    for (int i = 0; i < triangles.size(); i++) {
        Triangle *t = triangles[i];

        // Allocate new halfedges on heap
        Halfedge *h1 = new Halfedge();
        Halfedge *h2 = new Halfedge();
        Halfedge *h3 = new Halfedge();

        // Allocate new edges on heap
        Edge *e1 = new Edge();
        Edge *e2 = new Edge();
        Edge *e3 = new Edge();

        // Assign a halfedge pointer to the triangle
        t->halfedge = h1;

        // Assign halfedge pointers to point masses
        t->pm1->halfedge = h1;
        t->pm2->halfedge = h2;
        t->pm3->halfedge = h3;

        // Update all halfedge pointers
        h1->edge = e1;
        h1->next = h2;
        h1->pm = t->pm1;
        h1->triangle = t;

        h2->edge = e2;
        h2->next = h3;
        h2->pm = t->pm2;
        h2->triangle = t;

        h3->edge = e3;
        h3->next = h1;
        h3->pm = t->pm3;
        h3->triangle = t;
    }

    // Go back through the cloth mesh and link triangles together using halfedge
    // twin pointers

    // Convenient variables for math
    int num_height_tris = (num_height_points - 1) * 2;
    int num_width_tris = (num_width_points - 1) * 2;

    bool topLeft = true;
    for (int i = 0; i < triangles.size(); i++) {
        Triangle *t = triangles[i];

        if (topLeft) {
            // Get left triangle, if it exists
            if (i % num_width_tris != 0) { // Not a left-most triangle
                Triangle *temp = triangles[i - 1];
                t->pm1->halfedge->twin = temp->pm3->halfedge;
            } else {
                t->pm1->halfedge->twin = nullptr;
            }

            // Get triangle above, if it exists
            if (i >= num_width_tris) { // Not a top-most triangle
                Triangle *temp = triangles[i - num_width_tris + 1];
                t->pm3->halfedge->twin = temp->pm2->halfedge;
            } else {
                t->pm3->halfedge->twin = nullptr;
            }

            // Get triangle to bottom right; guaranteed to exist
            Triangle *temp = triangles[i + 1];
            t->pm2->halfedge->twin = temp->pm1->halfedge;
        } else {
            // Get right triangle, if it exists
            if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
                Triangle *temp = triangles[i + 1];
                t->pm3->halfedge->twin = temp->pm1->halfedge;
            } else {
                t->pm3->halfedge->twin = nullptr;
            }

            // Get triangle below, if it exists
            if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
                Triangle *temp = triangles[i + num_width_tris - 1];
                t->pm2->halfedge->twin = temp->pm3->halfedge;
            } else {
                t->pm2->halfedge->twin = nullptr;
            }

            // Get triangle to top left; guaranteed to exist
            Triangle *temp = triangles[i - 1];
            t->pm1->halfedge->twin = temp->pm2->halfedge;
        }

        topLeft = !topLeft;
    }

    clothMesh->triangles = triangles;
    this->clothMesh = clothMesh;
}

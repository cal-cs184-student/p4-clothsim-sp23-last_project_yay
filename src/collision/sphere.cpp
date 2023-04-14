#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
    Vector3D posDelta = pm.position - origin;
    if (posDelta.norm() > radius)
        return;

    Vector3D intersect = origin + posDelta.unit() * radius;
    Vector3D correction = intersect - pm.last_position;
    pm.position = pm.last_position + correction * (1 - friction);
}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}

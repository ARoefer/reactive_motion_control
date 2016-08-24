#include <ros/ros.h>
#include <eigen3/Eigen/Eigen>
#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>

#include <std_msgs/Float64.h>
#include <std_msgs/ColorRGBA.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

#include <r_libs/VisualizationManager.h>
#include <r_libs/R_Conversions.h>
#include <cmath>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "magnetic_controller/Core.h"
#include "Mesh.h"
#include "ObjectBase.h"

using namespace Eigen;
using namespace ros;
using namespace std;
using boost::property_tree::ptree;

template <class A, class B, class C>
C division(A a, B b) {
    return C(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

template <class A, class B, class C>
C multiply(A a, B b) {
    return C(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

std_msgs::ColorRGBA hsv2rgb(double h, double s, double v)
{
    double      hh, p, q, t, ff;
    long        i;
    std_msgs::ColorRGBA out;
    out.a = 1;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = v;
        out.g = v;
        out.b = v;
        return out;
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = v;
        break;
    case 5:
    default:
        out.r = v;
        out.g = p;
        out.b = q;
        break;
    }

    return out;     
}

void makeCubeMesh(Mesh* m, Vector3d dim, double resolution = 0.1) {
    Vector3d X(1,0,0);
    Vector3d Y(0,1,0);
    Vector3d Z(0,0,1);

    m->surfaces.push_back(Surface( X * 0.5 * dim[0],  X, dim[1] * dim[2]));
    m->surfaces.push_back(Surface(-X * 0.5 * dim[0], -X, dim[1] * dim[2]));
    m->surfaces.push_back(Surface( Y * 0.5 * dim[1],  Y, dim[0] * dim[2]));
    m->surfaces.push_back(Surface(-Y * 0.5 * dim[1], -Y, dim[0] * dim[2]));
    m->surfaces.push_back(Surface( Z * 0.5 * dim[2],  Z, dim[1] * dim[0]));
    m->surfaces.push_back(Surface(-Z * 0.5 * dim[2], -Z, dim[1] * dim[0]));

    m->dimensions = dim;
}

void makeVertStrip(Mesh* m, Vector3d edge, Vector3d pos) {
    double desStep = 0.1;

    int steps = (int)(edge.norm() / desStep);
    if (steps == 0)
        steps = 1;
    double step = 1.0 / steps;
    double radius = edge.norm() * step;

    cout << radius << endl;

    for (int i = 0; i <= steps; ++i)
    {
        m->surfaces.push_back(Surface(pos + step * i * edge, Vector3d(0,0,1), radius));
    }

    m->dimensions = edge;
}

void makeCubeMesh(ParticleCloud* cloud, Vector3d dim, double resolution = 0.1) {
    double dimX = dim[0], dimY = dim[1], dimZ = dim[2];

    cout << endl << "dim(" << dim[0] << ", " << dim[1] << ", " << dim[2] << ")" << endl;

    double desStep = resolution;
    Vector3i steps = (dim / desStep).cast<int>();

    Vector3d stepsNonZero(1,1,1);
    for (int i = 0; i < 3; ++i)
    {
        if (steps[i] != 0)
            stepsNonZero[i] = (double)steps[i];
    }

    Vector3d stepWidth = division<Vector3d, Vector3d, Vector3d>(dim, stepsNonZero);

    cout << "stepWidth(" << stepWidth[0] << ", " << stepWidth[1] << ", " << stepWidth[2] << ")" << endl;

    Vector3d baseNormal = Vector3d::Zero();
    Vector3d origin = -dim * 0.5;
    if (steps[0] == 0) {
        origin[0] = 0;
        baseNormal[0] = 1;
    }

    if (steps[1] == 0) {
        origin[1] = 0;
        baseNormal[1] = 1;
    }

    if (steps[2] == 0) {
        origin[2] = 0;
        baseNormal[2] = 1;
    }


    for (int x = 0; x <= steps[0]; ++x)
    {
        for (int y = 0; y <= steps[1]; ++y)
        {
            for (int z = 0; z <= steps[2]; ++z)
            {
                if (x == 0 || y == 0 || z == 0 || x == steps[0] || y == steps[1] || z == steps[2]) {
                    Vector3d normalX = Vector3d::Zero();
                    Vector3d normalY = Vector3d::Zero();
                    Vector3d normalZ = Vector3d::Zero();

                    if((x == 0 || x == steps[0]) && 0 != steps[0]) {
                        normalX = (Vector3d::UnitX() * (x== 0? -1 : 1));
                    }

                    if ((y == 0 || y == steps[1]) && 0 != steps[1]) {
                        normalY = (Vector3d::UnitY() * (y== 0? -1 : 1));
                    }

                    if ((z == 0 || z == steps[2]) && 0 != steps[2]) {
                        normalZ = (Vector3d::UnitZ() * (z== 0? -1 : 1));
                    }

                    SParticle particle;
                    particle.position = origin + multiply<Vector3d, Vector3d, Vector3d>(stepWidth, Vector3d(x,y,z));
                    particle.size = 1;//multiply<Vector3d, Vector3d, Vector3d>((baseNormal + normalX + normalY + normalZ), stepWidth).norm() * 0.5;
                    particle.normal = (baseNormal + normalX + normalY + normalZ).normalized();
                    cout << "(" << x << ", " << y << ", " << z << ") : "
                         << "p(" << particle.position[0] << ", " << particle.position[1] << ", " << particle.position[2] << ") " //<< endl
                         << "size(" << particle.size << ") " // << endl 
                         << "n(" << particle.normal[0] << ", " << particle.normal[1] << ", " << particle.normal[2] << ")" << endl;
                    cloud->particles.push_back(particle);
                }
            }
        }        
    }

    double partSize = (2 * dim[0] * dim[1] + 2 * dim[0] * dim[2] + 2 * dim[1] * dim[2]) / cloud->particles.size();

    for (int i = 0; i < cloud->particles.size(); ++i)
    {
        cloud->particles[i].size = partSize;
    }
}

void visualizeObjects(vector<ObjectBase<Mesh>*> &objects, vector<visualization_msgs::Marker> &markers, int ns, VisualizationManager &vis) {
    for (ObjectBase<Mesh>* pObj: objects) {
        markers.push_back(vis.shapeMarker(ns, 
                                         Affine3d::Identity(),
                                         visualization_msgs::Marker::CUBE, 
                                         pObj->pMesh->dimensions, 
                                         0.5f,0.5f,0.5f, 0.8f, 
                                         pObj->name));
    }//   
}

void makeXYRectangleMesh(ParticleCloud* cloud, Vector3d dim) {
    double dimX = dim[0], dimY = dim[1], dimZ = dim[2];

    cout << endl << "dim(" << dim[0] << ", " << dim[1] << ", " << dim[2] << ")" << endl;

    double desStep = 0.1;
    Vector3i steps = (dim / desStep).cast<int>();

    Vector3d stepsNonZero(1,1,1);
    for (int i = 0; i < 3; ++i)
    {
        if (steps[i] != 0)
            stepsNonZero[i] = (double)steps[i];
    }

    Vector3d stepWidth = division<Vector3d, Vector3d, Vector3d>(dim, stepsNonZero);

    cout << "stepWidth(" << stepWidth[0] << ", " << stepWidth[1] << ", " << stepWidth[2] << ")" << endl;

    Vector3d baseNormal = Vector3d::Zero();
    Vector3d origin = -dim * 0.5;
    if (steps[0] == 0) {
        origin[0] = 0;
        baseNormal[0] = 1;
    }

    if (steps[1] == 0) {
        origin[1] = 0;
        baseNormal[1] = 1;
    }

    for (int x = 0; x <= steps[0]; ++x)
    {
        for (int y = 0; y <= steps[1]; ++y)
        {
            if (x == 0 || y == 0 || x == steps[0] || y == steps[1]) {
                Vector3d normalX = Vector3d::Zero();
                Vector3d normalY = Vector3d::Zero();

                if((x == 0 || x == steps[0]) && 0 != steps[0]) {
                    normalX = (Vector3d::UnitX() * (x== 0? -1 : 1));
                }

                if ((y == 0 || y == steps[1]) && 0 != steps[1]) {
                    normalY = (Vector3d::UnitY() * (y== 0? -1 : 1));
                }

                SParticle particle;
                particle.position = origin + multiply<Vector3d, Vector3d, Vector3d>(stepWidth, Vector3d(x,y,0));
                particle.size = 1;//multiply<Vector3d, Vector3d, Vector3d>((baseNormal + normalX + normalY + normalZ), stepWidth).norm() * 0.5;
                particle.normal = (baseNormal + normalX + normalY).normalized();
                /*cout << "(" << x << ", " << y << ", " << ") : "
                     << "p(" << particle.position[0] << ", " << particle.position[1] << ", " << particle.position[2] << ") " //<< endl
                     << "size(" << particle.size << ") " // << endl 
                     << "n(" << particle.normal[0] << ", " << particle.normal[1] << ", " << particle.normal[2] << ")" << endl;//*/
                cloud->particles.push_back(particle);
            }
        }        
    }

    double partSize = (2 * dim[0] * dim[1] + 2 * dim[0] * dim[2] + 2 * dim[1] * dim[2]) / cloud->particles.size();

    for (int i = 0; i < cloud->particles.size(); ++i)
    {
        cloud->particles[i].size = partSize;
    }
}

void loadFromFile(ParticleCloud* cloud, string path) {
    ifstream file(path);

    if (file.is_open()) {
        while (!file.eof()) {
            string cmd;
            file >> cmd;

            if (cmd.compare("v") == 0) {
                double px, py, pz, nx, ny, nz, a;
                file >> px;
                file >> py;
                file >> pz;
                file >> nx;
                file >> ny;
                file >> nz;
                file >> a;

                SParticle particle;
                particle.position = Vector3d(px,py,pz);
                particle.normal = Vector3d(nx,ny,nz);
                particle.size = a;

                cloud->particles.push_back(particle);
            } else 
                cerr << "Unrecognized control '" << cmd << "'" << endl;
        }
    } else {
        cerr << "Opening of file '" << path << "' failed!" << endl;
    }
}


void visualizeObjects(vector<ObjectBase<ParticleCloud>*> &objects, vector<visualization_msgs::Marker> &markers, int ns, VisualizationManager &vis) {
    for (ObjectBase<ParticleCloud>* pObj: objects) {
        cout << "visualizing " << pObj->name << " with " << pObj->pMesh->particles.size() << " particles" << endl;
        visualization_msgs::Marker marker;
        marker.ns = vis.getNamespace(ns);
        if (pObj->statc) {
            marker.header.frame_id = "odom_combined";
            marker.pose = EigenToGeometrymsgs(pObj->getTransform());
        } else {
            marker.header.frame_id = pObj->name;
            marker.pose.orientation.w = 1.0;
        }
        marker.header.stamp = ros::Time::now();
        marker.id = vis.consumeId(ns);
        marker.type = visualization_msgs::Marker::POINTS;
        marker.scale.x = marker.scale.y = 0.04;

        marker.points.reserve(pObj->pMesh->particles.size());
        marker.colors.reserve(pObj->pMesh->particles.size());

        for (size_t i = 0; i < pObj->pMesh->particles.size(); i++) {
            geometry_msgs::Point point;
            point.x = pObj->pMesh->particles[i].position[0];
            point.y = pObj->pMesh->particles[i].position[1];
            point.z = pObj->pMesh->particles[i].position[2]; // Visual offset
            marker.points.push_back(point);

            Vector3d normal = pObj->pMesh->particles[i].normal;

            std_msgs::ColorRGBA color;
            color.r = 0.5 + normal[0] * 0.5;
            color.g = 0.5 + normal[1] * 0.5;
            color.b = 0.5 + normal[2] * 0.5;
            color.a = 1;
            marker.colors.push_back(color);

            //markers.push_back(vis.vectorMarker(ns, pObj->pMesh->particles[i].position, pObj->pMesh->particles[i].normal * 0.1, 1,0,0,1,0.01,0.02, marker.header.frame_id));
        }

        markers.push_back(marker);
    }//*/
}

Vector3d parsePosition(ptree &node) {
    ptree &attrs = node.get_child("<xmlattr>");
    return Vector3d(attrs.get<double>("posX"), attrs.get<double>("posY"), attrs.get<double>("posZ"));
}

Affine3d parseTransform(ptree &node) {
    ptree &attrs = node.get_child("<xmlattr>");
    Matrix3d m;
    m = AngleAxisd(attrs.get<double>("rotZ"), Vector3d::UnitZ())
      * AngleAxisd(attrs.get<double>("rotY"), Vector3d::UnitY())
      * AngleAxisd(attrs.get<double>("rotX"), Vector3d::UnitX());

    Quaterniond quat(m);
    Vector3d pos = parsePosition(node);
    Affine3d transform = Affine3d::Identity();
    transform.translate(pos);
    transform.rotate(quat);

    return transform;
}
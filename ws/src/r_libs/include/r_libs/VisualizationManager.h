#pragma once
#include <ros/ros.h>
#include <map>
#include <string>
#include <vector>
#include <visualization_msgs/Marker.h>
#include "eigen3/Eigen/Eigen"

using namespace Eigen;
using namespace std;

/* Class to manage ids and namespaces for visualization
 */
class VisualizationManager
{
public:
    // Add a namespace
    void addNamespace(int ns, string name);

    // Clear out a namespace
    void clearMarkerNS(int ns, ros::Publisher* publisher);
    // Clear out all namespaces
    void clearAllMarkers(ros::Publisher* publisher);

    void beginNewDrawCycle();
    void endDrawCycle(vector<visualization_msgs::Marker>& vis);

    visualization_msgs::Marker trailMarker(int ns, 
                                            const vector<Vector3d>& points, 
                                            double width = 0.1,
                                            float r = 0.f, 
                                            float g = 1.f, 
                                            float b = 0.f, 
                                            float a = 1.f, 
                                            string frame_id = "odom_combined");

    visualization_msgs::Marker shapeMarker(int ns, 
                                            Affine3d pose, 
                                            int shape, 
                                            Vector3d size,
                                            float r = 0.f, 
                                            float g = 1.f, 
                                            float b = 0.f, 
                                            float a = 1.f, 
                                            string frame_id = "odom_combined");

    visualization_msgs::Marker sphereMarker(int ns, 
                                            Vector3d center, 
                                            double radius, 
                                            float r = 1.f, 
                                            float g = 1.f, 
                                            float b = 1.f, 
                                            float a = 1.f, 
                                            string frame_id = "odom_combined");

    visualization_msgs::Marker vectorMarker(int ns, 
                                            Vector3d pos, 
                                            Vector3d vector, 
                                            float r = 1.f, 
                                            float g = 1.f, 
                                            float b = 1.f, 
                                            float a = 1.f, 
                                            double width = 0.01, 
                                            double headWidth = 0.02, 
                                            string frame_id = "odom_combined");

    visualization_msgs::Marker arrowMarker(int ns, 
                                            Vector3d start, 
                                            Vector3d end, 
                                            float r = 1.f, 
                                            float g = 1.f, 
                                            float b = 1.f, 
                                            float a = 1.f, 
                                            double width = 0.01, 
                                            double headWidth = 0.02, 
                                            string frame_id = "odom_combined");

    visualization_msgs::Marker textMarker(int ns, 
                                            Vector3d pos, 
                                            string text, 
                                            float r = 1.f, 
                                            float g = 1.f, 
                                            float b = 1.f, 
                                            float a = 1.f, 
                                            double height = 0.015, 
                                            string frame_id = "odom_combined");

    void annotatedVector(vector<visualization_msgs::Marker> &array,
                                            int ns, 
                                            Vector3d pos,
                                            Vector3d vector, 
                                            string text, 
                                            float r = 1.f, 
                                            float g = 1.f, 
                                            float b = 1.f, 
                                            float a = 1.f, 
                                            string frame_id = "odom_combined");

    void poseMarker(vector<visualization_msgs::Marker> &array, 
                    int ns, 
                    Affine3d pose, 
                    double arrowLength = 0.1, 
                    float a = 1.f, 
                    string frame_id = "odom_combined");

    visualization_msgs::Marker meshMarker(int ns, 
                                            Affine3d pose,
                                            Vector3d scale, 
                                            string resource, 
                                            string frame_id = "odom_combined", 
                                            Vector4f color = Vector4f::Zero()) {
        return meshMarker(ns, pose, scale, resource, frame_id, color[0], color[1], color[2], color[3]);
    }

    visualization_msgs::Marker meshMarker(int ns, 
                                            Affine3d pose,
                                            Vector3d scale, 
                                            string resource, 
                                            string frame_id = "odom_combined", 
                                            float r = 0, 
                                            float g = 0, 
                                            float b = 0, 
                                            float a = 0);

    int consumeId(int ns);
    string getNamespace(int ns);

private:
    geometry_msgs::Point vec2Point(Vector3d vec);
    geometry_msgs::Quaternion quat2QuatMsg(Quaterniond quat);
    
    map<int, string> namespaces;
    map<int,int> ids;
    map<int,int> lastIds;
};
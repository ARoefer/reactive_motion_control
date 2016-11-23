#include "r_libs/VisualizationManager.h"

#include <visualization_msgs/MarkerArray.h>

geometry_msgs::Point VisualizationManager::vec2Point(Vector3d vec)
{
geometry_msgs::Point p;
p.x = vec[0];
p.y = vec[1];
p.z = vec[2];

return p;
}

geometry_msgs::Quaternion VisualizationManager::quat2QuatMsg(Quaterniond quat)
{
geometry_msgs::Quaternion q;
q.x = quat.x();
q.y = quat.y();
q.z = quat.z();
q.w = quat.w();

return q;
}

void VisualizationManager::addNamespace(int ns, string name)
{
    namespaces[ns] = name;
    ids[ns] = 0;
}

void VisualizationManager::clearMarkerNS(int ns, ros::Publisher* publisher)
{
    visualization_msgs::MarkerArray out;

    visualization_msgs::Marker marker;
    marker.ns = namespaces[ns];
    marker.header.frame_id = "/odom_combined";
    marker.header.stamp = ros::Time::now();
    marker.action = visualization_msgs::Marker::DELETE;

    for (int i = 0; i < ids[ns]; ++i) {
        marker.id = i;
        out.markers.push_back(marker);
    }

    ids[ns] = 0;

    publisher->publish(out);
}

void VisualizationManager::clearAllMarkers(ros::Publisher* publisher)
{
    map<int, string>::iterator it = namespaces.begin();
    while(it != namespaces.end()) {
        clearMarkerNS((*it).first, publisher);
        it++;
    }

}

visualization_msgs::Marker VisualizationManager::sphereMarker(int ns, Vector3d center, double radius, float r, float g, float b, float a, string frame_id)
{
    visualization_msgs::Marker marker;
    marker.ns = namespaces[ns];
    marker.header.frame_id = frame_id;
    marker.header.stamp = ros::Time::now();
    marker.id = ids[ns];
    marker.type = visualization_msgs::Marker::SPHERE;
    marker.pose.position.x = center[0];
    marker.pose.position.y = center[1];
    marker.pose.position.z = center[2];
    marker.scale.x = radius;
    marker.scale.y = radius;
    marker.scale.z = radius;
    marker.color.r = r;
    marker.color.g = g;
    marker.color.b = b;
    marker.color.a = a;
    ids[ns]++;

    return marker;
}

visualization_msgs::Marker VisualizationManager::vectorMarker(int ns, Vector3d pos, Vector3d vector, float r, float g, float b, float a, double width, double headWidth, string frame_id) {
  return arrowMarker(ns, pos, pos+vector, r,g,b,a,width,headWidth,frame_id);
}

visualization_msgs::Marker VisualizationManager::arrowMarker(int ns, Vector3d start, Vector3d end, float r, float g, float b, float a, double width, double headWidth, string frame_id)
{
    visualization_msgs::Marker marker;
    marker.ns = namespaces[ns];
    marker.header.frame_id = frame_id;
    marker.header.stamp = ros::Time::now();
    marker.id = ids[ns];
    marker.type = visualization_msgs::Marker::ARROW;
    marker.points.push_back(vec2Point(start));
    marker.points.push_back(vec2Point(end));
    marker.scale.x = width;
    marker.scale.y = headWidth;
    marker.color.r = r;
    marker.color.g = g;
    marker.color.b = b;
    marker.color.a = a;
    ids[ns]++;

    return marker;
}

visualization_msgs::Marker VisualizationManager::textMarker(int ns, Vector3d pos, string text, float r, float g, float b, float a, double height, string frame_id)
{
    visualization_msgs::Marker marker;
    marker.ns = namespaces[ns];
    marker.header.frame_id = frame_id;
    marker.header.stamp = ros::Time::now();
    marker.id = ids[ns];
    marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
    marker.scale.z = height;
    marker.text = text;
    marker.color.r = r;
    marker.color.g = g;
    marker.color.b = b;
    marker.color.a = a;
    marker.pose.position = vec2Point(pos);
    ids[ns]++;

    return marker;
}

void VisualizationManager::annotatedVector(vector<visualization_msgs::Marker> &array, int ns, Vector3d pos, Vector3d vector, string text, float r, float g, float b, float a, string frame_id) {
  array.push_back(vectorMarker(ns, pos, vector,r,g,b,a));
  array.push_back(textMarker(ns, pos + vector * 0.5, text,r,g,b,a, 0.15));
}

visualization_msgs::Marker VisualizationManager::shapeMarker(int ns, Affine3d pose, int shape, Vector3d size, float r, float g, float b, float a, string frame_id) {
  
  visualization_msgs::Marker marker;
  marker.ns = namespaces[ns];
  marker.header.frame_id = frame_id;
  marker.header.stamp = ros::Time::now();
  marker.id = ids[ns];
  marker.type = shape;
    Vector3d translation = pose.translation();
    marker.pose.position.x = translation[0];
    marker.pose.position.y = translation[1];
    marker.pose.position.z = translation[2];

    Quaterniond ori(pose.rotation());
    marker.pose.orientation.x = ori.x();
    marker.pose.orientation.y = ori.y();
    marker.pose.orientation.z = ori.z();
    marker.pose.orientation.w = ori.w();

  marker.scale.x = size[0];
  marker.scale.y = size[1];
  marker.scale.z = size[2];
  
  marker.color.r = r;
  marker.color.g = g;
  marker.color.b = b;
  marker.color.a = a;

    ids[ns]++;

    return marker;
}

visualization_msgs::Marker VisualizationManager::trailMarker(int ns, const vector<Vector3d>& points, double width,float r, float g, float b, float a, string frame_id) {

  visualization_msgs::Marker marker;
  marker.ns = namespaces[ns];
  marker.header.frame_id = frame_id;
  marker.header.stamp = ros::Time::now();
  marker.id = ids[ns];
  marker.type = visualization_msgs::Marker::LINE_STRIP;

  marker.points.reserve(points.size());

  for (int i = 0; i < points.size(); i++) {
    geometry_msgs::Point point;
    point.x = points[i][0];
    point.y = points[i][1];
    point.z = points[i][2];
    marker.points.push_back(point);
  }

  marker.scale.x = width;

  marker.color.r = r;
  marker.color.g = g;
  marker.color.b = b;
  marker.color.a = a;

  ids[ns]++;

  return marker;
}

visualization_msgs::Marker VisualizationManager::meshMarker(int ns, Affine3d pose, Vector3d scale, string resource, string frame_id, float r, float g, float b, float a) {
      visualization_msgs::Marker marker;
  marker.ns = namespaces[ns];
  marker.header.frame_id = frame_id;
  marker.header.stamp = ros::Time::now();
  marker.id = ids[ns];
  marker.mesh_resource = resource;
  marker.mesh_use_embedded_materials = true;
  marker.type = visualization_msgs::Marker::MESH_RESOURCE;
    Vector3d translation = pose.translation();
    marker.pose.position.x = translation[0];
    marker.pose.position.y = translation[1];
    marker.pose.position.z = translation[2];

    Quaterniond ori(pose.rotation());
    marker.pose.orientation.x = ori.x();
    marker.pose.orientation.y = ori.y();
    marker.pose.orientation.z = ori.z();
    marker.pose.orientation.w = ori.w();

  marker.scale.x = scale[0];
  marker.scale.y = scale[1];
  marker.scale.z = scale[2];

  marker.color.r = r;
  marker.color.g = g;
  marker.color.b = b;
  marker.color.a = a;

    ids[ns]++;

    return marker;
}

void VisualizationManager::poseMarker(vector<visualization_msgs::Marker> &array, int ns, Affine3d pose, double arrowLength, float a, string frame_id)
{
    Vector3d pos(pose.matrix().block(0,3,3,1));

    array.push_back(arrowMarker(ns, pos, pos + Vector3d(pose.matrix().block(0,0,3,1)) * arrowLength, 1, 0, 0, a, 0.01, 0.01, frame_id));
    array.push_back(arrowMarker(ns, pos, pos + Vector3d(pose.matrix().block(0,1,3,1)) * arrowLength, 0, 1, 0, a, 0.01, 0.01, frame_id));
    array.push_back(arrowMarker(ns, pos, pos + Vector3d(pose.matrix().block(0,2,3,1)) * arrowLength, 0, 0, 1, a, 0.01, 0.01, frame_id));
}

int VisualizationManager::consumeId(int ns) {
  ids[ns]++;
  return ids[ns]-1;
}

string VisualizationManager::getNamespace(int ns) {
  return namespaces[ns];
}

void VisualizationManager::beginNewDrawCycle() {
  lastIds = ids;
  for (auto it = ids.begin(); it != ids.end(); it++) {
    ids[it->first] = 0;
  }
}

void VisualizationManager::endDrawCycle(vector<visualization_msgs::Marker>& vis) {
  for (auto it = ids.begin(); it != ids.end(); it++) {
    for (int i = it->second; i < lastIds[it->first]; ++i)
    {
      visualization_msgs::Marker marker;
      marker.ns = namespaces[it->first];
      marker.header.stamp = ros::Time::now();
      marker.id = i;
      marker.action = visualization_msgs::Marker::DELETE;
      vis.push_back(marker);
    }
  }
}
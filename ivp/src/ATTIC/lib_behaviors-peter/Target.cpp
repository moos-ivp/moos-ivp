#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "Target.h"
#include "GeometricUtils.h"

using namespace std;

Target::Target() {
  ID = "unknown";
  x = 0;
  y = 0;
  z = 0;
  speed = 0;
  heading = 0;
}

Target::Target(string id) {
  ID = id;
}

Target::~Target() {
}

string Target::getID() {
  return ID;
}

void Target::setID(string id) {
  ID = id;
}

double Target::getX() const {
  return x;
}

void Target::setX(double x) {
  this->x = x;
}
  
double Target::getY() const {
  return y;
}

void Target::setY(double y) {
  this->y = y;
}

double Target::getZ() const {
  return z;
}

void Target::setZ(double z) {
  this->z = z;
}

double Target::getSpeed() const {
  return speed;
}

void Target::setSpeed(double speed) {
  this->speed = speed;
}
  
double Target::getHeading() const {
  return heading;
}

void Target::setHeading(double heading) {
  this->heading = heading;
}

bool Target::isTooClose(double targetRange, double desiredTargetRange)
  const {
  if(targetRange < desiredTargetRange)
    return true;
 
  return false;
}

bool Target::isTooFar(double targetRange, double desiredTargetRange) const {
  if(targetRange > desiredTargetRange)
    return true;

  return false;
}

bool Target::isWithinRange(double targetRange, double targetRangeMin,
      double targetRangeMax) const {
  if(!isTooClose(targetRange, targetRangeMin) &&
      !isTooFar(targetRange, targetRangeMax))
    return true;

  return false;
}

bool Target::isToTheRightOf(double x, double y, double offset){
  if(this->x > (x + offset))
    return true;

  return false;
}

bool Target::isToTheLeftOf(double x, double y, double offset){
  if(this->x < (x - offset))
    return true;

  return false;
}

bool Target::isAbove(double x, double y, double offset){
  if(this->y > (y + offset))
    return true;

  return false;
}

bool Target::isBelow(double x, double y, double offset){
  if(this->y < (y - offset))
    return true;

  return false;
}

bool Target::isMovingAway(double x, double y){
  if(speed == 0)
    return false;

  if(isToTheRightOf(x, y, 0) && isAbove(x, y, 0))
    if((heading >= 180) && (heading <= 270))
      return false;

  if(isToTheRightOf(x, y, 0) && isBelow(x, y, 0))
    if((heading >= 270) && (heading <= 360 || heading == 0))
      return false;

  if(isToTheLeftOf(x, y, 0) && isBelow(x, y, 0))
    if((heading >= 0 || heading == 360) && (heading <= 90))
      return false;

  if(isToTheLeftOf(x, y, 0) && isAbove(x, y, 0))
    if((heading >= 90) && (heading <= 180))
      return false;

  return true;
}

bool Target::isMovingToward(double x, double y){
  if(speed == 0)
    return false;

  if(isMovingAway(x, y))
    return false;

  return true;
}

bool Target::isInNorthWestSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left - l <= this->x && this->x <= left)
    if(top <= this->y && this->y <= top + l)
      return true;

  return false;
}

bool Target::isInNorthSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left <= this->x && this->x <= right)
    if(top <= this->y && this->y <= top + l)
      return true;

  return false;
}

bool Target::isInNorthEastSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(right <= this->x && this->x <= right + l)
    if(top <= this->y && this->y <= top + l)
      return true;

  return false;
}

bool Target::isInEastSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(right <= this->x && this->x <= right + l)
    if(bottom <= this->y && this->y <= top)
      return true;

  return false;
}

bool Target::isInSouthEastSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(right <= this->x && this->x <= right + l)
    if(bottom - l <= this->y && this->y <= bottom)
      return true;

  return false;
}

bool Target::isInSouthSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left <= this->x && this->x <= right)
    if(bottom - l <= this->y && this->y <= bottom)
      return true;

  return false;
}

bool Target::isInSouthWestSquare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);
  
  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left - l <= this->x && this->x <= left)
    if(bottom - l <= this->y && this->y <= bottom)
      return true;

  return false;
}

bool Target::isInWestSuare(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left - l <= this->x && this->x <= right)
    if(bottom <= this->y && this->y <= top)
      return true;

  return false;
}

bool Target::isNorthWestOf(double x, double y, double l) {
//  if(isInNorthWestSquare(x, y, l))
//    return false;

  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(this->x <= left)
    if(this->y >= top)
      return true;

  return false;
}

bool Target::isNorthOf(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left <= this->x && this->x <= right)
//    if(this->y >= top + l)
    if(this->y >= top)
      return true;

  return false;
}

bool Target::isNorthEastOf(double x, double y, double l) {
//  if(isInNorthEastSquare(x, y, l))
//    return false;

  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(this->x >= right)
    if(this->y >= top)
      return true;

  return false;
}

bool Target::isEastOf(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

//  if(this->x >= right + l)
  if(this->x >= right)
    if(bottom <= this->y && this->y <= top)
      return true;

  return false;
}

bool Target::isSouthEastOf(double x, double y, double l) {
//  if(isInSouthEastSquare(x, y, l))
//    return false;

  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(this->x >= right)
    if(this->y <= bottom)
      return true;

  return false;
}

bool Target::isSouthOf(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(left <= this->x && this->x <= right)
//    if(this->y <= bottom - l)
    if(this->y <= bottom)
      return true;

  return false;
}

bool Target::isSouthWestOf(double x, double y, double l) {
//  if(isInSouthWestSquare(x, y, l))
//    return false;

  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(this->x <= left)
    if(this->y <= bottom)
      return true;

  return false;
}

bool Target::isWestOf(double x, double y, double l) {
  double a = floor(x/l);
  double b = floor(y/l);

  double left = a*l;
  double bottom = b*l;

  double right = left + l;
  double top = bottom + l;

  if(this->x <= left)
    if(bottom <= this->y && this->y <= top)
      return true;

  return false;
}

bool Target::isMovingNorthWest() {
  if(292.5 < heading && heading <= 337.5)
    return true;

  return false;
}

bool Target::isMovingNorth() {
  if((337.5 < heading && heading <= 360) || (0 < heading && heading <= 22.5))
    return true;

  return false;
}

bool Target::isMovingNorthEast() {
  if(22.5 < heading && heading <= 67.5)
    return true;

  return false;
}

bool Target::isMovingEast() {
  if(67.5 < heading && heading <= 112.5)
    return true;

  return false;
}

bool Target::isMovingSouthEast() {
  if(112.5 < heading && heading <= 157.5)
    return true;

  return false;
}

bool Target::isMovingSouth() {
  if(157.5 < heading && heading <= 202.5)
    return true;

  return false;
}

bool Target::isMovingSouthWest() {
  if(202.5 < heading && heading <= 247.5)
    return true;

  return false;
}

bool Target::isMovingWest() {
  if(247.5 < heading && heading <= 292.5)
    return true;

  return false;
}

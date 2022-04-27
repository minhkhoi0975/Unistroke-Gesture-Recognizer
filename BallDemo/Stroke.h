// Stroke.h
// Programmer: Khoi Ho

#pragma once

#include <string>
#include <vector>
#include "Vector2.h"

class Stroke
{
public:
    // The name of the stroke
    std::string name;

    // The points of the stroke
    std::vector<Vector2> points;

	// Constructors.
	Stroke();
	Stroke(const std::string& name);
	Stroke(const Stroke& other);

	// Assignment operator overloading.
	Stroke& operator= (const Stroke& other);

	// Equal operator overloading.
	bool operator== (const Stroke& other);

	// Less than operator overloading.
	bool operator< (const Stroke& other);

	// Get the centroid of the stroke.
	Vector2 GetCentroid() const;

	// Get the bounding box of the stroke.
	void GetBoundingBox(Vector2& topLeftCorner, Vector2& bottomRightCorner) const;

	// Find the total length of the lines between the points.
	float GetPathLength() const;

	// Resample the stroke.
	Stroke Resample(int numPoints = 64) const;

	// Find the indicative angle from the centroid to the first point.
	float GetIndicativeAngle() const;

	// Rotate the stroke by an angle.
	Stroke RotateBy(float angle) const;

	// Scale the stroke to a (size x size) bounding box.
	Stroke ScaleTo(const int& size = 250) const;

	// Translate a stroke to the origin.
	Stroke TranslateTo(const Vector2& origin = Vector2()) const;

	// Get the average distance between respective points of the 2 strokes.
	float GetPathDistance(const Stroke& other) const;

	// ???
	float GetDistanceAtAngle(const Stroke& other, const float& angle) const;

	// ??? 
	float GetDistanceAtBestAngle(const Stroke& other, float angleAlpha, float angleBeta, const float& angleDelta) const;

	// Find the stroke that matches this stroke and calculate the score.
	void Recognize(std::vector<Stroke>& strokeTemplates, const float& size, Stroke& matchingStroke, float& score) const;
};


// Stroke.h
// Programmer: Khoi Ho

#pragma once

#include <string>
#include <vector>
#include "Vector2.h"

// Reference: http://faculty.washington.edu/wobbrock/pubs/uist-07.01.pdf
class Stroke
{
public:
    // The name of the stroke.
    std::string name;

    // The points of the stroke.
    std::vector<Vector2> points;

	Stroke();
	Stroke(const std::string& name);
	Stroke(const Stroke& other);

	// Used for copying values instead of copying the reference.
	Stroke& operator= (const Stroke& other);

	// Used for checking if the two strokes share the same values instead of the same reference.
	bool operator== (const Stroke& other);

	// Used for sorting strokes by name.
	bool operator< (const Stroke& other);

	// Returns the centroid of the stroke.
	Vector2 GetCentroid() const;

	// Returns the bounding box of the stroke.
	void GetBoundingBox(Vector2& topLeftCorner, Vector2& bottomRightCorner) const;

	// Returns the total length of the stroke.
	float GetPathLength() const;

	// Resamples the points into the specified number of evenly spaced points.
	Stroke Resample(int numPoints = 64) const;

	// Finds the indicative angle from the first point of the stroke to the centroid.
	float GetIndicativeAngle() const;

	// Rotates the stroke by an angle around the centroid.
	Stroke RotateBy(float angle) const;

	// Scales the stroke to match the bounding box. The size parameter is the size of each side of the bounding box.
	Stroke ScaleTo(const int& size = 250) const;

	// Translates the stroke to the origin.
	Stroke TranslateTo(const Vector2& origin = Vector2()) const;

	// Returns the average distance between respective points of the 2 strokes.
	float GetPathDistance(const Stroke& other) const;

	// Returns the distance between this stroke, rotated by an angle, and the other stroke.
	float GetDistanceAtAngle(const Stroke& other, const float& angle) const;

	// ??? 
	float GetDistanceAtBestAngle(const Stroke& other, float angleAlpha, float angleBeta, const float& angleDelta) const;

	// Finds the stroke that matches this stroke and returns the score.
	void Recognize(std::vector<Stroke>& strokeTemplates, const float& size, Stroke& matchingStroke, float& score) const;
};


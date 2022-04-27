#include "Stroke.h"
#include <limits>

Stroke::Stroke():name(std::string()) {}

Stroke::Stroke(const std::string& name):name(name) {}

Stroke::Stroke(const Stroke& other):name(other.name), points(other.points) {}

Stroke& Stroke::operator=(const Stroke& other)
{
	this->name = other.name;
	this->points = other.points;

	return *this;
}

bool Stroke::operator==(const Stroke& other)
{
	return this->name == other.name && this->points == other.points;
}

bool Stroke::operator<(const Stroke& other)
{
	return this->name < other.name;
}

Vector2 Stroke::GetCentroid() const
{
	if (points.size() == 0)
		throw std::exception("Cannot find the centroid: The stroke has no point.");

	float sumX = 0;
	float sumY = 0;
	int pointCount = points.size();

	for (int i = 0; i < pointCount; ++i)
	{
		sumX += points[i].x;
		sumY += points[i].y;
	}

	return Vector2(sumX / pointCount, sumY / pointCount);
}

void Stroke::GetBoundingBox(Vector2& topLeftCorner, Vector2& bottomRightCorner) const
{
	if (points.size() == 0)
		throw std::exception("Cannot find the bounding box: The stroke has no point.");

	float minX = points[0].x;
	float minY = points[0].y;
	float maxX = points[0].x;
	float maxY = points[0].y;

	for (int i = 1; i < points.size(); ++i)
	{
		if (points[i].x < minX)
		{
			minX = points[i].x;
		}
		else if (points[i].x > maxX)
		{
			maxX = points[i].x;
		}

		if (points[i].y < minY)
		{
			minY = points[i].y;
		}
		else if (points[i].y > maxY)
		{
			maxY = points[i].y;
		}
	}

	topLeftCorner.x = minX;
	topLeftCorner.y = minY;

	bottomRightCorner.x = maxX;
	bottomRightCorner.y = maxY;
}

float Stroke::GetPathLength() const
{
	if (points.size() <= 1)
		return 0.0f;

	float length = 0;

	for (int i = 1; i < points.size(); ++i)
	{
		length += Vector2::Distance(points[i - 1], points[i]);
	}

	return length;
}

Stroke Stroke::Resample(int numPoints) const
{
	if (points.size() == 0)
		throw std::exception("Cannot resample the stroke: The stroke has no point.");

	// Create a copy stroke. We don't want to change the original points.
	Stroke strokeCopy(*this);

	Stroke resampledStroke(name);
	resampledStroke.points.reserve(numPoints);

	float I = strokeCopy.GetPathLength() / (numPoints - 1);
	float D = 0;

	// Insert the first point of the original stroke into the sampled stroke.
	resampledStroke.points.push_back(strokeCopy.points[0]);

	for (int i = 1; i < strokeCopy.points.size(); ++i)
	{
		float d = Vector2::Distance(strokeCopy.points[i - 1], strokeCopy.points[i]);

		if (D + d >= I)
		{
			Vector2 newPoint;
			newPoint.x = strokeCopy.points[i - 1].x + ((I - D) / d) * (strokeCopy.points[i].x - strokeCopy.points[i - 1].x);
			newPoint.y = strokeCopy.points[i - 1].y + ((I - D) / d) * (strokeCopy.points[i].y - strokeCopy.points[i - 1].y);

			resampledStroke.points.push_back(newPoint);

			strokeCopy.points.insert(strokeCopy.points.begin() + i, newPoint);

			D = 0;
		}
		else
		{
			D += d;
		}
	}

	// Fix the bug in which the size of newPoints does not match numPoints.
	if (resampledStroke.points.size() < numPoints)
	{
		resampledStroke.points.push_back(points[points.size() - 1]);
	}

	return resampledStroke;
}

float Stroke::GetIndicativeAngle() const
{
	if (points.size() == 0)
		throw std::exception("Cannot find the indicative angle: The stroke has no point.");

	Vector2 centroid = GetCentroid();
	return atan2(centroid.y - points[0].y, centroid.x - points[0].x);
}

Stroke Stroke::RotateBy(float angle) const
{
	if (points.size() == 0)
		throw std::exception("Cannot rotate the stroke: The stroke has no point.");

	Stroke newStroke(name);
	newStroke.points.reserve(this->points.size());

	Vector2 centroid = GetCentroid();

	for (const Vector2& point : points)
	{
		Vector2 newPoint;
		newPoint.x = (point.x - centroid.x) * cos(angle) - (point.y - centroid.y) * sin(angle) + centroid.x;
		newPoint.y = (point.x - centroid.x) * sin(angle) + (point.y - centroid.y) * cos(angle) + centroid.y;

		newStroke.points.push_back(newPoint);
	}

	return newStroke;
}

Stroke Stroke::ScaleTo(const int& size) const
{
	if (points.size() == 0)
		throw std::exception("Cannot scale the stroke: The stroke has no point.");

	Stroke newStroke(name);
	newStroke.points.reserve(this->points.size());

	// Get the bounding box of the points.
	Vector2 topLeftCorner;
	Vector2 bottomRightCorner;
	GetBoundingBox(topLeftCorner, bottomRightCorner);

	// Get the size of the bounding box.
	float width = bottomRightCorner.x - topLeftCorner.x;
	float height = bottomRightCorner.y - topLeftCorner.y;

	for (const Vector2& point : points)
	{
		Vector2 newPoint;
		newPoint.x = point.x * size / width;
		newPoint.y = point.y * size / height;

		newStroke.points.push_back(newPoint);
	}

	return newStroke;
}

Stroke Stroke::TranslateTo(const Vector2& origin) const
{
	if (points.size() == 0)
		throw std::exception("Cannot translate the stroke: The stroke has no point.");

	Stroke newStroke(name);
	newStroke.points.reserve(this->points.size());

	Vector2 centroid = GetCentroid();

	for (const Vector2& point : points)
	{
		Vector2 newPoint;
		newPoint.x = point.x + origin.x - centroid.x;
		newPoint.y = point.y + origin.y - centroid.y;

		newStroke.points.push_back(newPoint);
	}

	return newStroke;
}

float Stroke::GetPathDistance(const Stroke& other) const
{
	if (this->points.size() != other.points.size())
	{
		throw std::exception("Error: Cannot find the path distance: The two strokes have different sizes.");
	}
	else if (points.size() == 0)
	{
		throw std::exception("Error: Cannot find the path distance: Both strokes do not have any points.");
	}

	float distance = 0;

	for (int i = 0; i < this->points.size(); ++i)
	{
		distance += Vector2::Distance(this->points[i], other.points[i]);
	}

	return distance / points.size();
}

float Stroke::GetDistanceAtAngle(const Stroke& other, const float& angle) const
{
	Stroke rotatedStroke = RotateBy(angle);
	float distance = rotatedStroke.GetPathDistance(other);
	return distance;
}

float Stroke::GetDistanceAtBestAngle(const Stroke& other, float angleAlpha, float angleBeta, const float& angleDelta) const
{
	static const float phi = 0.5f * (-1.0f + sqrt(5.0f));

	float x1 = phi * angleAlpha + (1.0f - phi) * angleBeta;
	float f1 = GetDistanceAtAngle(other, x1);

	float x2 = (1.0f - phi) * angleAlpha + phi * angleBeta;
	float f2 = GetDistanceAtAngle(other, x2);

	while (abs(angleBeta - angleAlpha) > angleDelta)
	{
		if (f1 < f2)
		{
			angleBeta = x2;
			x2 = x1;
			f2 = f1;
			x1 = phi * angleAlpha + (1.0f - phi) * angleBeta;
			f1 = GetDistanceAtAngle(other, x1);
		}
		else
		{
			angleAlpha = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1.0f - phi) * angleAlpha + phi * angleBeta;
			f2 = GetDistanceAtAngle(other, x2);
		}
	}

	return f1 < f2 ? f1 : f2;
}

void Stroke::Recognize(std::vector<Stroke>& strokeTemplates, const float& size, Stroke& matchingStroke, float& score) const
{
	static const float PI = 2.0f * acosf(0.0f);
	static const float ANGLE_ALPHA = -0.25f * PI; //  45 degrees.
	static const float ANGLE_BETA = 0.25f * PI;   // -45 degrees.
	static const float ANGLE_DELTA = PI / 90.0f;  //   2 degrees.

	float bestDistance = std::numeric_limits<float>::infinity();

	for (Stroke& strokeTemplate : strokeTemplates)
	{
		float distance = GetDistanceAtBestAngle(strokeTemplate, ANGLE_ALPHA, ANGLE_BETA, ANGLE_DELTA);

		if (distance < bestDistance)
		{
			bestDistance = distance;
			matchingStroke = strokeTemplate;
		}

		score = 1.0f - bestDistance / (0.5f * sqrt(size * size + size * size));
	}
}
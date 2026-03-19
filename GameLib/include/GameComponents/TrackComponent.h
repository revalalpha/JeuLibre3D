#pragma once
#include "Core/Spline.h"
#include "Core/Frenet.h"

/**
 * @brief Component representing a track defined by a Hermite curve.
 */
struct TrackComponent
{
	HermitCurve curve;					   ///< The Hermite curve defining the track's centerline.
	std::vector<glm::vec3> sampledPoints;  ///< Pre-sampled points along the curve for efficient access.
	std::vector<glm::vec3> forwardDirs;    ///< Precomputed forward (tangent) directions at sampled points.
	std::vector<KGR::CurveFrame> frames;   ///< Precomputed Frenet frames (forward, up, right) at sampled points for orientation.
	float step = 0.1f;					   ///< Sampling step size along the curve (smaller = more points, smoother but more expensive).
	float trackWidth = 5.0f;               ///< Width of the track, used for rendering and collision.
};
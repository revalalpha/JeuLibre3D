#pragma once

#include <vector>
#include <glm/glm.hpp>

/*
	Example how to use the RMF to build a Frenet frame along a curve defined by a set of points:

	std::vector<glm::vec3> points = ...; // Your curve points
	std::vector<glm::vec3> forwardDirs = KGR::RMF::EstimateForwardDirs(points);
	std::vector<KGR::CurveFrame> frames = KGR::RMF::BuildFrames(points, forwardDirs);

*/

namespace KGR
{
    struct CurveFrame
    {
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;
    };

	namespace RMF
	{
		/*
			The MovingFrame function computes the next frame based on the previous frame and the current segment of the curve.
			The function uses the reflection method to compute the next frame, 
			ensuring that the forward direction is correctly aligned with the curve while minimizing twisting.

			- previousFrame: The frame at the previous point on the curve.
			- from: The starting point of the current segment.
			- to: The ending point of the current segment.
			- nextForward: The forward direction at the next point on the curve.

			The function returns the next frame, which includes the forward, up, and right vectors that define the orientation of the curve at the next point.
		*/
		CurveFrame MovingFrame(const CurveFrame& previousFrame,
			const glm::vec3& from,
			const glm::vec3& to,
			const glm::vec3& nextForward);

		/*
			The BuildFrames function constructs a sequence of frames along the curve defined by the input points and their corresponding forward directions.
			The result is a vector of CurveFrame objects, each representing the orientation of the curve at a specific point.

			- points: A vector of points defining the curve.
			- forwardDirs: A vector of forward directions corresponding to each point on the curve. Must be the same size as points.

			The function returns a vector of CurveFrame. Each frame is computed using the MovingFrame function, 
			starting from an initial frame at the first point and iteratively building the frames along the curve.
		*/
		std::vector<CurveFrame> BuildFrames(const std::vector<glm::vec3>& points,
			const std::vector<glm::vec3>& forwardDirs);

		/*
			Surcharged functions to estimate forward directions based on the positions of the points on the curve. 
			These are used to provide initial forward directions for the BuildFrames function.
			You can use these functions to estimate the forward direction for each point on the curve based on its position relative to its neighbors:

			 - EstimateFirstForwardDir: Estimates the forward direction for the first point using the first and second points.
			 - EstimateMiddleForwardDir: Estimates the forward direction for a middle point using the previous, current, and next points.
			 - EstimateLastForwardDir: Estimates the forward direction for the last point using the last and second-to-last points.

			The functions returns a normalized forward direction vector that can be used as input for the BuildFrames function.
		*/
		glm::vec3 EstimateFirstForwardDir(const glm::vec3& current, const glm::vec3& next);
		glm::vec3 EstimateMiddleForwardDir(const glm::vec3& prev, const glm::vec3& current, const glm::vec3& next);
		glm::vec3 EstimateLastForwardDir(const glm::vec3& prev, const glm::vec3& current);

		/*
			The EstimateForwardDirs function computes the forward directions for a sequence of points on the curve. 
			It uses the surcharged functions to estimate the forward direction for each point based on its position relative to its neighbors.

			- points: A vector of points defining the curve. Must contain at least 2 points.

			The function returns a vector of forward directions corresponding to each point on the curve, which can be used as input for the BuildFrames function.
		*/
		std::vector<glm::vec3> EstimateForwardDirs(const std::vector<glm::vec3>& points);
	}
}
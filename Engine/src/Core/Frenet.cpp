#include "Core/Frenet.h"
#include <stdexcept>

KGR::CurveFrame KGR::RMF::MovingFrame(const CurveFrame& previousFrame, const glm::vec3& from, const glm::vec3& to,
										 const glm::vec3& nextForward)
{
    glm::vec3 segmentDirection = glm::normalize(to - from);

	// Reflection formula: v' = v - 2 * dot(v, n) * n
    glm::vec3 reflectionForward = previousFrame.forward - 2.0f * glm::dot(previousFrame.forward, segmentDirection) * segmentDirection;
    glm::vec3 reflectionUp = previousFrame.up - 2.0f * glm::dot(previousFrame.up, segmentDirection) * segmentDirection;

    glm::vec3 correctionAxis = nextForward - reflectionForward;
    float     correctionSquare = glm::dot(correctionAxis, correctionAxis);

    glm::vec3 movingUp = (correctionSquare < 1e-10f) ? reflectionUp : reflectionUp - (2.0f / correctionSquare) * glm::dot(correctionAxis, reflectionUp) * correctionAxis;
    
    KGR::CurveFrame nextFrame;
    nextFrame.forward = nextForward;
    nextFrame.up = glm::normalize(movingUp);
    nextFrame.right = glm::normalize(glm::cross(nextFrame.forward, nextFrame.up));

    return nextFrame;
}

glm::vec3 KGR::RMF::EstimateFirstForwardDir(const glm::vec3& current, const glm::vec3& next)
{
    return glm::normalize(next - current);
}

glm::vec3 KGR::RMF::EstimateMiddleForwardDir(const glm::vec3& prev, const glm::vec3& current, const glm::vec3& next)
{
    return glm::normalize(next - prev);
}

glm::vec3 KGR::RMF::EstimateLastForwardDir(const glm::vec3& prev, const glm::vec3& current)
{
    return glm::normalize(current - prev);
}

std::vector<glm::vec3> KGR::RMF::EstimateForwardDirs(const std::vector<glm::vec3>& points)
{
        const std::size_t pointCount = points.size();

        if (pointCount < 2)
            throw std::invalid_argument("at least 2 points required");

        std::vector<glm::vec3> forwardDirs(pointCount);

        forwardDirs[0]              = EstimateFirstForwardDir(points[0], points[1]);
        forwardDirs[pointCount - 1] = EstimateLastForwardDir(points[pointCount - 2], points[pointCount - 1]);

        for (std::size_t i = 1; i < pointCount - 1; ++i)
            forwardDirs[i] = EstimateMiddleForwardDir(points[i - 1], points[i], points[i + 1]);

        return forwardDirs;
}


std::vector<KGR::CurveFrame> KGR::RMF::BuildFrames(const std::vector<glm::vec3>& points,
                                             const std::vector<glm::vec3>& forwardDirs)
{
        const std::size_t pointCount = points.size();

        if (pointCount < 2 || forwardDirs.size() != pointCount)
            throw std::invalid_argument("points and forwardDirs must have the same size (>= 2)");

        std::vector<CurveFrame> frames(pointCount);
        {
            const glm::vec3& firstForward = forwardDirs[0];

				glm::vec3 worldAxis = (std::abs(firstForward.y) < 0.9f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);

			// Gram-Schmidt process: u' = u - proj_v(u) where proj_v(u) = (dot(u, v) / dot(v, v)) * v
            glm::vec3 firstUp = glm::normalize(worldAxis - glm::dot(worldAxis, firstForward) * firstForward);

            frames[0].forward = firstForward;
            frames[0].up = firstUp;
            frames[0].right = glm::normalize(glm::cross(firstForward, firstUp));
        }

        for (std::size_t i = 0; i < pointCount - 1; ++i)
            frames[i + 1] = MovingFrame(frames[i], points[i], points[i + 1], forwardDirs[i + 1]);

        return frames;
}
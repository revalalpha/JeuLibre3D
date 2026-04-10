#include "ScoreManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "glm/glm.hpp"

void ScoreManager::Run(float driftFactor, float speed, float dt)
{
	float factor = driftFactor;
	if(driftFactor < 0.f)
		factor= driftFactor*-1;

	
	scoreInvincibility -= dt;
	if (factor > 4.8f && speed > 1.f)
	{
		points += (( factor * m_driftScoreRate * std::max(1.f,(speed / 4.f)) * dt )/15.f)*m_multiplier;
		AddScore(points);
		time += dt;
		ComputeMult(dt);
		SetHighScore();
		cancelTimer = 3.f;
	}
	else
	{
		ResetScore(dt);
	}
	
}

void ScoreManager::wallHit(float speed)
{
	if (scoreInvincibility <= 0.f)
	{
		time = 0.f;
		m_multiplier = 1.f;
		m_score = 0;
		points = 0;
		scoreInvincibility = 0.3;
	}
}

void ScoreManager::AddScore(float points)
{
	m_score = static_cast<int>(points);
}

void ScoreManager::ComputeMult(float dt)
{
	m_multiplier = 1 + std::pow(time, 0.5);
}

void ScoreManager::ResetScore(float dt)
{
	cancelTimer -= dt;

	if (cancelTimer <= 0.0f)
	{
		time = 0.f;
		m_score = 0;
		points = 0;
		m_multiplier = 1.f;
		cancelTimer = 3.f;
	}
}

void ScoreManager::SetHighScore()
{
	if (m_highScore < m_score)
		m_highScore = m_score;
}
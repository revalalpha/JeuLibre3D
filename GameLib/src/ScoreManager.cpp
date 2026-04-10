#include "ScoreManager.h"

void ScoreManager::Update(float driftFactor, float speed, float dt)
{
	if (driftFactor > 0.0f)
	{
		int points = static_cast<int>((driftFactor * speed) * m_driftScoreRate * dt);
		AddScore(points);
	}
	else
	{
		ResetScore(dt);
	}
	SetHighScore();
}

void ScoreManager::AddScore(int points)
{
	m_score += points * m_multiplier;
	ComputeMult();
}

void ScoreManager::ComputeMult()
{
	m_multiplier = 1 + (m_score / 100);
}

void ScoreManager::ResetScore(float dt)
{
	cancelTimer -= dt;

	if (cancelTimer <= 0.0f)
	{
		m_score = 0;
		cancelTimer = 3.0f;
	}
}

void ScoreManager::SetHighScore()
{
	if (m_highScore < m_score)
		m_highScore = m_score;
}
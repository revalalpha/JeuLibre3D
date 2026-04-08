#pragma once

class ScoreManager
{
public:
	static ScoreManager& GetInstance()
	{
		static ScoreManager instance;
		return instance;
	}

	void Run(float driftFactor, float speed,float dt);

	int GetScore() const {
		return m_score;
	}

private:
	ScoreManager() = default;
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;

	void ComputeMult();
	void AddScore(int points);
	void ResetScore(float dt);

	int m_score = 0;
	int m_multiplier = 1;
	float m_driftScoreRate = 100.0f;

	float cancelTimer = 3.0f;
};

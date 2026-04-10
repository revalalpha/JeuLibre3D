#pragma once

class ScoreManager
{
public:
	ScoreManager() = default;

	static ScoreManager& GetInstance()
	{
		static ScoreManager instance;
		return instance;
	}

	void Run(float driftFactor, float speed,float dt);
	void wallHit(float speed);

	int GetScore() const {
		return m_score;
	}
	int GetMultiplier() const {
		return m_multiplier;
	}
	int GetHighScore() const {
		return m_highScore;
	}

private:
	/*ScoreManager() = default;
	ScoreManager(const ScoreManager&) = delete;
	ScoreManager& operator=(const ScoreManager&) = delete;*/

	void ComputeMult(float dt);
	void AddScore(float points);
	void ResetScore(float dt);
	void SetHighScore();


	int m_highScore = 0;
	int m_score = 0;
	float points = 0;
	int m_multiplier = 1;
	float m_driftScoreRate = 10.0f;
	float time = 0.f;

	float cancelTimer = 3.f;
	float scoreInvincibility = 0.3;
};

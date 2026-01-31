#pragma once

struct PlayerConfig {
    float superSpeed = 60.0f;
    float superJump = 420.0f;
    float gravityScale = 1.0f;
    float playerMass = 100.0f;
    float groundFriction = 8.0f;
    float punchForce = 1.0f;
    float kickForce = 1.0f;
    float damageMultiplier = 1.0f;
    float musclePower = 0.7f;
    float heightRate = 1.0f;
    float muscleRate = 1.0f;
    float drunkLevel = 0.0f;
    bool godModeEnabled = false;
    bool infiniteStamina = false;
    bool noPain = false;
    bool maxConsciousness = false;
    bool zombieMode = false;
    bool infiniteKick = false;
    float debugHealth = 100.0f;
};

struct EnemyConfig {
    float healAmount = 50.0f;
    float damageAmount = 50.0f;
};

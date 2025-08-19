#pragma once

class PbrRenderer {
public:
    PbrRenderer();
    ~PbrRenderer();

    PbrRenderer(const PbrRenderer&) = delete;
    PbrRenderer(PbrRenderer&&) = delete;
    PbrRenderer& operator=(const PbrRenderer&) = delete;
    PbrRenderer& operator=(PbrRenderer&&) = delete;

private:
};
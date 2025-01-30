#include "BezierCurve.h"
#include <cmath>

void BezierCurve::addPoint(glm::vec3 point) {
  if (points.size() == MAX_BEZIER_POINTS) {
    points.clear();
  } else {
    points.push_back(point);
  }
  updatePoints();
}

void BezierCurve::clearPoints() {
  points.clear();
  updatePoints();
}

static std::vector<int> computeBinominalFactors(int n) {
  std::vector<int> factors(n+1);
  int a = 1;
  for (int i = 1; i <= n + 1; i++) {
    factors[i - 1] = a;
    a = a * (n - i + 1) / i;
  }

  return factors;
}

void BezierCurve::updatePoints() {
  // Update polygon
  if (points.size() == 0) {
    polygonCurve.setPoints({});
    approximatedCurve.setPoints({});
    interpolatedCurve.setPoints({});
    return;
  } else if (points.size() == 1) {
    polygonCurve.setPoints(points);
    approximatedCurve.setPoints({});
    interpolatedCurve.setPoints({});
    return;
  }
  polygonCurve.setPoints(points);

  // approximate points
  int numSamples = points.size() * 10;
  int n = points.size() - 1;
  float t, b;
  std::vector<glm::vec3> approximatedPoints(numSamples);

  std::vector<int> factors = computeBinominalFactors(n);
  for (int i = 0; i < numSamples; i++) {
    t = 1.0 / (numSamples - 1) * i;
    glm::vec3 point{0.0f, 0.0f, 0.0f};
    for (int j = 0; j <= n; j++) {
      if (j == 0) {
        b = factors[j] * pow(1 - t, n);
      } else if (j == n) {
        b = factors[j] * pow(t, n);
      } else {
        b = factors[j] * pow(t, j) * pow(1 - t, n - j);
      }
      point += b * points[j];
    }
    approximatedPoints[i] = point;
  }
  approximatedCurve.setPoints(approximatedPoints);

  // interpolated points
  if (points.size() >= 3) {
    std::vector<glm::vec3> interpolatedPoints(numSamples);
    glm::mat4 T = {
                  0,             0,     0, 1,
      pow(1/3.f, 3), pow(1/3.f, 2), 1/3.f, 1,
      pow(2/3.f, 3), pow(2/3.f, 2), 2/3.f, 1,
                  1,             1,     1, 1,
    };

    glm::mat4 Tinv = glm::inverse(T);
    glm::mat4x3 Points = {
        points[points.size() - 3],
        points[points.size() - 3],
        points[points.size() - 2],
        points[points.size() - 1],
      };
    if (points.size() >= 4) {
      Points[0] = points[points.size() - 4];
    }
    for (int i = 0; i < numSamples; i++) {
      t = 1.0 / (numSamples - 1) * i;

      if (points.size() == 3) {
        t = 1.0 - 2/3.0 * t;
      }

      glm::vec4 ts {t*t*t, t*t, t, 1};
      glm::vec3 point = Points * Tinv * ts;
      interpolatedPoints[i] = point;
    }
    interpolatedCurve.setPoints(interpolatedPoints);
  }
}

glm::vec3 BezierCurve::getAproximated(float t) {
  int n = points.size() - 1;
  float b;

  std::vector<int> factors = computeBinominalFactors(n);
  glm::vec3 point{0.0f, 0.0f, 0.0f};
  for (int j = 0; j <= n; j++) {
    if (j == 0) {
      b = factors[j] * pow(1 - t, n);
    } else if (j == n) {
      b = factors[j] * pow(t, n);
    } else {
      b = factors[j] * pow(t, j) * pow(1 - t, n - j);
    }
    point += b * points[points.size() - 1 - j];
  }
  return point;
}

glm::vec3 BezierCurve::getInterpolated(float t) {
  if (points.size() < 3) {
    return glm::vec3(0,0,0);
  }

  if (points.size() == 3) {
    t = 1/3.0 + 2/3.0 * t;
  }

  glm::mat4 T = {
                0,             0,     0, 1,
    pow(1/3.f, 3), pow(1/3.f, 2), 1/3.f, 1,
    pow(2/3.f, 3), pow(2/3.f, 2), 2/3.f, 1,
                1,             1,     1, 1,
  };

  glm::mat4 Tinv = glm::inverse(T);
  glm::mat4x3 Points = {
      points[points.size() - 3],
      points[points.size() - 3],
      points[points.size() - 2],
      points[points.size() - 1],
  };
  if (points.size() >= 4) {
    Points[0] = points[points.size() - 4];
  }
  glm::vec4 ts {t*t*t, t*t, t, 1};
  return Points * Tinv * ts;
}

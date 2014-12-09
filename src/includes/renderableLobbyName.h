#pragma once

#include <renderablePanel.h>
#include <glm/gtx/constants.hpp>
#include <stdio.h>

class RenderableLobbyName : public RenderablePanel {
public:
  RenderableLobbyName() : RenderablePanel() {};
  ~RenderableLobbyName() = default;

  void update(int offset, int totalNames, float aspectRatio) {
    aspectRatio = aspectRatio > MAX_RATIO ? MAX_RATIO : aspectRatio;

    float width = aspectRatio*TEXT_PANEL_HEIGHT;
    float height = TEXT_PANEL_HEIGHT;
    int totalColumns = ceil(((float)totalNames)/NAMES_PER_COLUMN);
    int columnOffset = offset/NAMES_PER_COLUMN;
    offset %= NAMES_PER_COLUMN;

    bool oddNumberOfColumns = totalColumns % 2;
    float phi;
    if (oddNumberOfColumns) {
      int centerColumn = totalColumns/2;
      int distToCenter = centerColumn - columnOffset;
      phi = distToCenter*glm::pi<float>()*2.0f/NUMBER_OF_COLUMNS;
    } else { // even number of columns
      float betweenCenterColumns = ((float)(totalColumns - 1))/2;
      float distToCenter = betweenCenterColumns - columnOffset;
      phi = distToCenter*glm::pi<float>()*2.0f/NUMBER_OF_COLUMNS;
    }

    glm::mat4 phiRot = glm::rotate(glm::degrees(phi), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 thetaRot = glm::rotate(
      glm::degrees(COLUMN_START_ANGLE - VERTICAL_OFFSET_ANGLE*(offset + 1)),
      glm::vec3(1.0f, 0.0f, 0.0f)
    );

    glm::mat4 rotMat = phiRot*thetaRot;

    // corners
    glm::vec3 topLeft(-width/2.0f, 1.0f, TEXT_PANEL_HEIGHT);
    glm::vec3 topRight = topLeft + glm::vec3(width, 0.0f, 0.0f);
    glm::vec3 bottomLeft = topLeft + glm::vec3(0.0f, 0.0f, -height);
    glm::vec3 bottomRight = topLeft + glm::vec3(width, 0.0f, -height);

    topLeft = glm::vec3(rotMat*glm::vec4(topLeft, 0.0f));
    topRight = glm::vec3(rotMat*glm::vec4(topRight, 0.0f));
    bottomLeft = glm::vec3(rotMat*glm::vec4(bottomLeft, 0.0f));
    bottomRight = glm::vec3(rotMat*glm::vec4(bottomRight, 0.0f));

    createVertices(topLeft, topRight, bottomLeft, bottomRight);
    RenderablePanel::update = true;
  };

private:
  const int MAX_PLAYERS = 100;
  const int NAMES_PER_COLUMN = 20;
  const float COLUMN_START_ANGLE = glm::half_pi<float>()*0.75f;
  const float COLUMN_END_ANGLE = glm::half_pi<float>()*0.25f;

  const float COLUMN_ANGLE = COLUMN_START_ANGLE - COLUMN_END_ANGLE;
  const int NUMBER_OF_COLUMNS = MAX_PLAYERS/NAMES_PER_COLUMN;
  const float MAX_NAME_WIDTH = cos(COLUMN_START_ANGLE)*glm::pi<float>()*2.0f/(NUMBER_OF_COLUMNS);
  const float VERTICAL_OFFSET_ANGLE = COLUMN_ANGLE/NAMES_PER_COLUMN;
  const float TEXT_PANEL_HEIGHT = VERTICAL_OFFSET_ANGLE;
  const float MAX_RATIO = MAX_NAME_WIDTH/TEXT_PANEL_HEIGHT;
};

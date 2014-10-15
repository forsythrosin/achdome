class DomeSurface;
struct GameState;

class Scene {
public:
  Scene(DomeSurface *domeSurface, std::string fragShader, std::string vertShader);
  Scene();
  ~Scene();

  /**
   * Calls OpelGL functions for this scene
   */
  void render();

  void setDomesurface(DomeSurface *domeSurface);
  void setShaders(std::string fragShader, std::string vertShader);
  void setGameState(GameState &gs);

  const DomeSurface * const getDomeSurface() const;
  const int getWormCount() const;
  const int getGameState() const;

private:
  DomeSurface *domeSurface;
  std::string fragShader, vertShader;
};

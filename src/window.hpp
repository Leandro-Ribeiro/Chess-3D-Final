#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "model.hpp"
//#include "trackball.cpp"

#define TILE_SIZE 0.128f

struct Coord{
  int X, Y;
};

struct Piece{
    char type; //k = Rei, q = Dama, b = Bispo, n = Cavalo, r = Torre, p = Peão
    int color;

    glm::vec3 pos;
    int X, Y;

    std::vector<Coord> legalMoves;

    bool captured = false;
    bool hasMoved = false;
};

struct Tile{
  int X;
  int Y;
  int color;

  bool exists = false;
  int pieceID = -1;

  glm::vec3 pos;
};

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  Model m_tile;
  Model m_pawn;
  Model m_rook;
  Model m_knight;
  Model m_bishop;
  Model m_queen;
  Model m_king;

  Tile board[8][8];
  std::vector<Piece> pieces;
  Coord kingPos[2];
  bool whiteKingChecked = false;
  bool blackKingChecked = false;

  bool gameOver = false;
  bool whiteWin = false;
  bool blackWin = false;
  bool draw = false;

  int turn = 0;

  int selectedX = 0;
  int selectedY = 0;
  bool movingPiece = false;
  int movingPieceID = -1;

  Coord lastStart = {-1,-1};
  Coord lastEnd = {-1,-1};

  //TrackBall m_trackBallLight;
  float m_zoom = 45.0f;

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  bool rotating = false;
  float rotationAngle = 0;

  std::vector<char const *> m_shaderNames{"texture", "blinnphong", "phong", "gouraud",
                                          "normal", "depth"};
  std::vector<GLuint> m_programs;
  int m_currentProgramIndex{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{};

  std::vector<Piece> setupBoard();
  void resetBoard();

  bool isLegal(Coord target);
  void getAllLegalMoves();
  std::vector<Coord> getLegalMoves(Piece p);
  bool isThreatened(Coord target, int color);
  std::vector<Coord> checkChecks(Piece p, std::vector<Coord> candidates);

  bool checkForGameOver();

  void renderModel(Model m, int color, glm::vec3 pos);
  void renderBoard();
  void renderPieces();

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f};
  float m_shininess{150.0f};
};

#endif
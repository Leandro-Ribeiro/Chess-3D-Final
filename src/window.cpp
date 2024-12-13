#include "window.hpp"
#include "imfilebrowser.h"

void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  if (event.type == SDL_MOUSEMOTION) {
    //m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_RIGHT) {
      //m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_RIGHT) {
      //m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_KEYDOWN && !rotating && !gameOver){
    switch(event.key.keysym.sym){
      case SDLK_ESCAPE:
      resetBoard();
      break;
      case SDLK_RIGHT:
      if(turn == 0){
        selectedX++;
        if(selectedX > 7) selectedX = 0;
      }
      else if(turn == 1){
        selectedX--;
        if(selectedX < 0) selectedX = 7;
      }
      break;
      case SDLK_LEFT:
      if(turn == 0){
        selectedX--;
        if(selectedX < 0) selectedX = 7;
      }
      else if(turn == 1){
        selectedX++;
        if(selectedX > 7) selectedX = 0;
      }
      break;
      case SDLK_UP:
      if(turn == 1){
        selectedY++;
        if(selectedY > 7) selectedY = 0;
      }
      else if(turn == 0){
        selectedY--;
        if(selectedY < 0) selectedY = 7;
      }
      break;
      case SDLK_DOWN:
      if(turn == 1){
        selectedY--;
        if(selectedY < 0) selectedY = 7;
      }
      else if(turn == 0){
        selectedY++;
        if(selectedY > 7) selectedY = 0;
      }
      break;
      case SDLK_SPACE:
      if(!movingPiece && board[selectedX][selectedY].exists && pieces[board[selectedX][selectedY].pieceID].color == turn){
        movingPiece = true;
        movingPieceID = board[selectedX][selectedY].pieceID;
        pieces[movingPieceID].pos.y+=0.2f;

        //legalMoves = getLegalMoves(pieces[movingPieceID]);
      }
      else if(movingPiece){
        if((isLegal({selectedX,selectedY}))){
          board[pieces[movingPieceID].X][pieces[movingPieceID].Y].exists = false;
          board[pieces[movingPieceID].X][pieces[movingPieceID].Y].pieceID = -1;
          lastStart.X = pieces[movingPieceID].X;
          lastStart.Y = pieces[movingPieceID].Y;

          pieces[movingPieceID].X = selectedX;
          pieces[movingPieceID].Y = selectedY;
          pieces[movingPieceID].pos.y-=0.2f;
          lastEnd.X = pieces[movingPieceID].X;
          lastEnd.Y = pieces[movingPieceID].Y;

          //Captures Piece
          if(board[selectedX][selectedY].exists){
              pieces[board[selectedX][selectedY].pieceID].captured = true;
          }
          //En Passant
          if(pieces[movingPieceID].type == 'p' && lastEnd.X != lastStart.X && !board[lastEnd.X][lastEnd.Y].exists){
              if(pieces[movingPieceID].color == 0){
              pieces[board[lastEnd.X][lastEnd.Y+1].pieceID].captured = true;
              board[lastEnd.X][lastEnd.Y+1].exists = false;
              board[lastEnd.X][lastEnd.Y+1].pieceID = -1;
              }
              if(pieces[movingPieceID].color == 1){
              pieces[board[lastEnd.X][lastEnd.Y-1].pieceID].captured = true;
              board[lastEnd.X][lastEnd.Y-1].exists = false;
              board[lastEnd.X][lastEnd.Y-1].pieceID = -1;
              }
          }
          //Promotion
          if(pieces[movingPieceID].type == 'p' && (lastEnd.Y == 0 || lastEnd.Y == 7)){
              pieces[movingPieceID].type = 'q';
          }
          //Castling
          if(pieces[movingPieceID].type == 'k' && !pieces[movingPieceID].hasMoved && (lastEnd.X == 2 || lastEnd.X == 6)){
              switch(lastEnd.X){
                  case 2:
                  board[3][lastEnd.Y].exists = true;
                  board[3][lastEnd.Y].pieceID = board[0][lastEnd.Y].pieceID;

                  pieces[board[0][lastEnd.Y].pieceID].X = 3;
                  pieces[board[0][lastEnd.Y].pieceID].pos = board[3][lastEnd.Y].pos;
                  pieces[board[0][lastEnd.Y].pieceID].pos.y += 0.001;

                  board[0][lastEnd.Y].exists = false;
                  board[0][lastEnd.Y].pieceID = -1;
                  break;
                  case 6:
                  board[5][lastEnd.Y].exists = true;
                  board[5][lastEnd.Y].pieceID = board[7][lastEnd.Y].pieceID;

                  pieces[board[7][lastEnd.Y].pieceID].X = 5;
                  pieces[board[7][lastEnd.Y].pieceID].pos = board[5][lastEnd.Y].pos;
                  pieces[board[7][lastEnd.Y].pieceID].pos.y += 0.001;

                  board[7][lastEnd.Y].exists = false;
                  board[7][lastEnd.Y].pieceID = -1;
                  break;
              }
          }
          
          board[selectedX][selectedY].exists = true;
          board[selectedX][selectedY].pieceID = movingPieceID;

          if(pieces[movingPieceID].type == 'k'){
            kingPos[pieces[movingPieceID].color] = lastEnd;
          }

          movingPiece = false;
          pieces[movingPieceID].hasMoved = true;
          movingPieceID = -1;

          turn = turn==0? 1:0;
          
          if(turn == 0 && isThreatened(kingPos[0], 0)) whiteKingChecked = true;
          else whiteKingChecked = false; 
          if(turn == 1 && isThreatened(kingPos[1], 1)) blackKingChecked = true;
          else blackKingChecked = false; 

          getAllLegalMoves();
          
          if(checkForGameOver()) {
            gameOver = true;
            if(turn == 0 && whiteKingChecked) blackWin = true;
            else if(turn == 1 && blackKingChecked) whiteWin = true;
            else draw = true;
            break;
          }
          rotating = true;
        }
        else{
          pieces[movingPieceID].pos = board[pieces[movingPieceID].X][pieces[movingPieceID].Y].pos;
          pieces[movingPieceID].pos.y += 0.001;

          movingPiece = false;
          movingPieceID = -1;
        }
      }
      break;
      case SDLK_r:
      rotating = true;
      break;
    }    
  }

  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += event.wheel.y < 0 ? -5.0f : 5.0f;
    m_zoom = glm::clamp(m_zoom, 0.0f, 90.0f);
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Create programs
  for (auto const &name : m_shaderNames) {
    auto const program{
        abcg::createOpenGLProgram({{.source = assetsPath + name + ".vert",
                                    .stage = abcg::ShaderStage::Vertex},
                                   {.source = assetsPath + name + ".frag",
                                    .stage = abcg::ShaderStage::Fragment}})};
    m_programs.push_back(program);
  }


  m_pawn.loadObj(assetsPath + "pawn_hd.obj",false);
  m_pawn.setupVAO(m_programs.at(m_currentProgramIndex));
  m_pawn.loadDiffuseTexture(assetsPath + "Table_tex.jpg");
  m_rook.loadObj(assetsPath + "rook_hd.obj",false);
  m_rook.setupVAO(m_programs.at(m_currentProgramIndex));
  m_rook.loadDiffuseTexture(assetsPath + "Table_tex.jpg");
  m_knight.loadObj(assetsPath + "knight_hd.obj",false);
  m_knight.setupVAO(m_programs.at(m_currentProgramIndex));
  m_knight.loadDiffuseTexture(assetsPath + "Table_tex.jpg");
  m_bishop.loadObj(assetsPath + "bishop_hd.obj",false);
  m_bishop.setupVAO(m_programs.at(m_currentProgramIndex));
  m_bishop.loadDiffuseTexture(assetsPath + "Table_tex.jpg");
  m_queen.loadObj(assetsPath + "queen_hd.obj",false);
  m_queen.setupVAO(m_programs.at(m_currentProgramIndex));
  m_queen.loadDiffuseTexture(assetsPath + "Table_tex.jpg");
  m_king.loadObj(assetsPath + "king_hd.obj",false);
  m_king.setupVAO(m_programs.at(m_currentProgramIndex));
  m_king.loadDiffuseTexture(assetsPath + "Table_tex.jpg");

  m_tile.loadObj(assetsPath + "tile.obj",false);
  m_tile.setupVAO(m_programs.at(m_currentProgramIndex));
  m_tile.loadDiffuseTexture(assetsPath + "Table_tex.jpg");

  m_mappingMode = 1;
  abcg::glEnable(GL_CULL_FACE);
  
  float x,y;
  x = y = -4*TILE_SIZE + TILE_SIZE/2;
  for(int i=0;i<8;i++)
    for(int j=0;j<8;j++){
      board[i][j].X = i;
      board[i][j].Y = j;
      board[i][j].color = (i+j)%2==0? 2:3;
      board[i][j].pos = {x+(i*TILE_SIZE),0.0f,y+(j*TILE_SIZE)};
    }

  pieces = setupBoard();
  getAllLegalMoves();

}

void Window::onUpdate() {
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 1.5f),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  m_viewMatrix = glm::rotate(m_viewMatrix, glm::radians(m_zoom), glm::vec3(1, 0, 0));

  if(!rotating){
    if((turn == 1 && !gameOver) || (turn == 0 && gameOver)) m_viewMatrix = glm::rotate(m_viewMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
  }
  //Moves moving piece to selected square
  if(movingPiece){
    pieces[movingPieceID].pos = board[selectedX][selectedY].pos;
    pieces[movingPieceID].pos.y += 0.201f;
  }

  //Rotates the board around
  if(rotating){
    m_viewMatrix = glm::rotate(m_viewMatrix, glm::radians(rotationAngle), glm::vec3(0, 1, 0));
    rotationAngle+=5;

    if(turn == 1 && rotationAngle == 180){
      rotating = false;
    }
    else if (turn == 0 && rotationAngle == 360){
        rotating = false;
        rotationAngle = 0;
    }
  }
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use currently selected program
  auto const program{m_programs.at(m_currentProgramIndex)};
  abcg::glUseProgram(program);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};
  m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

  abcg::glUseProgram(m_programs.at(m_currentProgramIndex));

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(program, "projMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};
  auto const IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(program, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(program, "Is")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(program, "diffuseTex")};
  auto const mappingModeLoc{abcg::glGetUniformLocation(program, "mappingMode")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(mappingModeLoc, m_mappingMode);

  auto const lightDirRotated{glm::mat4{1.0f} * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  // Render Models

  renderBoard();
  renderPieces();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  // Create a window for instructions
  {
    auto const widgetSize{ImVec2(300, 100)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Controles", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Setas - Mover casa selecionada");
    ImGui::Text("Espaço - Levantar/Soltar peça da casa selecionada");
    ImGui::Text("Roda do Mouse - Ajustar Ângulo de Visão");

    ImGui::End();
  }

  // Create a window for end game
  if(gameOver){
    {
        auto const widgetSize{ImVec2(300, 100)};
        auto windowWidth = 300;
        auto windowHeight = 100;
        ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x/2 - windowWidth/2, m_viewportSize.y/2 - windowHeight/2 - 20));
        ImGui::SetNextWindowSize(widgetSize);
        ImGui::Begin("", nullptr, ImGuiWindowFlags_NoDecoration);
        auto textWidth = ImGui::CalcTextSize("Fim de Jogo!").x;
        ImGui::SetCursorPosX((windowWidth-textWidth)/2);
        ImGui::Text("Fim de Jogo!");
        if (whiteWin){
          textWidth = ImGui::CalcTextSize("Brancas vencem por cheque-mate!").x;
          ImGui::SetCursorPosX((windowWidth-textWidth)/2);
          ImGui::Text("Brancas vencem por cheque-mate!");
        }
        else if (blackWin){
          textWidth = ImGui::CalcTextSize("Pretas vencem por cheque-mate!").x;
          ImGui::SetCursorPosX((windowWidth-textWidth)/2);
          ImGui::Text("Pretas vencem por cheque-mate!");
        }
        else if (draw){
          textWidth = ImGui::CalcTextSize("Empate por afogamento!").x;
          ImGui::SetCursorPosX((windowWidth-textWidth)/2);
          ImGui::Text("Empate por afogamento!");
        }
        textWidth = ImGui::CalcTextSize("Jogar Novamente").x;
        ImGui::SetCursorPosX((windowWidth-textWidth)/2);
        if (ImGui::Button("Jogar Novamente")) {
          resetBoard();
        }
        ImGui::End();
    }
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
}

std::vector<Piece> Window::setupBoard(){
  Piece temp;
  std::vector<Piece> pieces;

  float offsetY = 0.001;

  //Rooks
  temp = {'r',1,board[0][0].pos,0,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[0][0].exists = true;
  board[0][0].pieceID = pieces.size()-1;

  temp = {'r',1,board[7][0].pos,7,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[7][0].exists = true;
  board[7][0].pieceID = pieces.size()-1;

  temp = {'r',0,board[0][7].pos,0,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[0][7].exists = true;
  board[0][7].pieceID = pieces.size()-1;

  temp = {'r',0,board[7][7].pos,7,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[7][7].exists = true;
  board[7][7].pieceID = pieces.size()-1;

  //Knights
  temp = {'n',1,board[1][0].pos,1,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[1][0].exists = true;
  board[1][0].pieceID = pieces.size()-1;

  temp = {'n',1,board[6][0].pos,6,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[6][0].exists = true;
  board[6][0].pieceID = pieces.size()-1;

  temp = {'n',0,board[1][7].pos,1,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[1][7].exists = true;
  board[1][7].pieceID = pieces.size()-1;

  temp = {'n',0,board[6][7].pos,6,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[6][7].exists = true;
  board[6][7].pieceID = pieces.size()-1;

  //Bishops
  temp = {'b',1,board[2][0].pos,2,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[2][0].exists = true;
  board[2][0].pieceID = pieces.size()-1;

  temp = {'b',1,board[5][0].pos,5,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[5][0].exists = true;
  board[5][0].pieceID = pieces.size()-1;

  temp = {'b',0,board[2][7].pos,2,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[2][7].exists = true;
  board[2][7].pieceID = pieces.size()-1;

  temp = {'b',0,board[5][7].pos,5,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[5][7].exists = true;
  board[5][7].pieceID = pieces.size()-1;

  //Queens
  temp = {'q',1,board[3][0].pos,3,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[3][0].exists = true;
  board[3][0].pieceID = pieces.size()-1;

  temp = {'q',0,board[3][7].pos,3,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[3][7].exists = true;
  board[3][7].pieceID = pieces.size()-1;

  //Kings
  temp = {'k',1,board[4][0].pos,4,0};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[4][0].exists = true;
  board[4][0].pieceID = pieces.size()-1;
  kingPos[1] = {4,0};

  temp = {'k',0,board[4][7].pos,4,7};
  temp.pos.y += offsetY;
  pieces.push_back(temp);
  board[4][7].exists = true;
  board[4][7].pieceID = pieces.size()-1;
  kingPos[0] = {4,7};

  //Pawns
  for(int i=0; i<8; i++){
      temp = {'p',1,board[i][1].pos,i,1};
      temp.pos.y += offsetY;
      pieces.push_back(temp);
      board[i][1].exists = true;
      board[i][1].pieceID = pieces.size()-1;

      temp = {'p',0,board[i][6].pos,i,6};
      temp.pos.y += offsetY;
      pieces.push_back(temp);
      board[i][6].exists = true;
      board[i][6].pieceID = pieces.size()-1;
    }

    return pieces;
}

void Window::resetBoard(){
  gameOver = whiteWin = blackWin = draw = false;
  turn = 0;
  blackKingChecked = whiteKingChecked = false;
  selectedX = 0;
  selectedY = 0;
  movingPiece = false;
  movingPieceID = -1;

  lastStart = {-1,-1};
  lastEnd = {-1,-1};

  pieces.clear();

  float x,y;
  x = y = -4*TILE_SIZE + TILE_SIZE/2;
  for(int i=0;i<8;i++)
    for(int j=0;j<8;j++){
      board[i][j].X = i;
      board[i][j].Y = j;
      board[i][j].color = (i+j)%2==0? 2:3;
      board[i][j].pos = {x+(i*TILE_SIZE),0.0f,y+(j*TILE_SIZE)};
      board[i][j].exists = false;
      board[i][j].pieceID = -1;
    }

  pieces = setupBoard();

  getAllLegalMoves();
}

bool Window::isLegal(Coord target){
  for(int i=0; i<pieces[movingPieceID].legalMoves.size(); i++){
      if((target.X == pieces[movingPieceID].legalMoves[i].X) && (target.Y == pieces[movingPieceID].legalMoves[i].Y)) return true;
  }
  return false;
}

void Window::getAllLegalMoves(){
  for(int i=0;i<pieces.size();i++){
    if(pieces[i].color == turn && !pieces[i].captured){
      movingPieceID = i;
      pieces[i].legalMoves = getLegalMoves(pieces[movingPieceID]);
      movingPieceID = -1;
    }
  }
}

std::vector<Coord> Window::getLegalMoves(Piece p){
  std::vector<Coord> legal;
  Coord temp;

  switch(p.type){
    //Pawn
    case 'p':
    if(p.color == 0){
        if(!board[p.X][p.Y-1].exists){
            temp.X = p.X;
            temp.Y = p.Y-1;
            legal.push_back(temp);
            if(!board[p.X][p.Y-2].exists && !p.hasMoved){
            temp.X = p.X;
            temp.Y = p.Y-2;
            legal.push_back(temp);
            }
        }

        //Capture
        if(p.X-1 >= 0 && board[p.X-1][p.Y-1].exists && pieces[board[p.X-1][p.Y-1].pieceID].color == 1){
            temp.X = p.X-1;
            temp.Y = p.Y-1;
            legal.push_back(temp);
        }
        if(p.X+1 <= 7 && board[p.X+1][p.Y-1].exists && pieces[board[p.X+1][p.Y-1].pieceID].color == 1){
            temp.X = p.X+1;
            temp.Y = p.Y-1;
            legal.push_back(temp);
        }

        //En Passant
        if(p.X-1 >= 0 && board[p.X-1][p.Y].X == lastEnd.X && board[p.X-1][p.Y].Y == 3 && lastStart.Y == 1){
            if(pieces[board[p.X-1][p.Y].pieceID].type == 'p' && pieces[board[p.X-1][p.Y].pieceID].color == 1){
                temp.X = p.X-1;
                temp.Y = p.Y-1;
                legal.push_back(temp);
            }
        }
        else if(p.X+1 <= 7 && board[p.X+1][p.Y].X == lastEnd.X && board[p.X+1][p.Y].Y == 3 && lastStart.Y == 1){
            if(pieces[board[p.X+1][p.Y].pieceID].type == 'p' && pieces[board[p.X+1][p.Y].pieceID].color == 1){
                temp.X = p.X+1;
                temp.Y = p.Y-1;
                legal.push_back(temp);
            }
        }
    }
    else{
        if(!board[p.X][p.Y+1].exists){
            temp.X = p.X;
            temp.Y = p.Y+1;
            legal.push_back(temp);
            if(!board[p.X][p.Y+2].exists && !p.hasMoved){
            temp.X = p.X;
            temp.Y = p.Y+2;
            legal.push_back(temp);
            } 
        }

        //Capture
        if(p.X-1 >= 0 && board[p.X-1][p.Y+1].exists && pieces[board[p.X-1][p.Y+1].pieceID].color == 0){
            temp.X = p.X-1;
            temp.Y = p.Y+1;
            legal.push_back(temp);
        }
        if(p.X+1 <= 7 && board[p.X+1][p.Y+1].exists && pieces[board[p.X+1][p.Y+1].pieceID].color == 0){
            temp.X = p.X+1;
            temp.Y = p.Y+1;
            legal.push_back(temp);
        }

        //En Passant
        if(p.X-1 >= 0 && board[p.X-1][p.Y].X == lastEnd.X && board[p.X-1][p.Y].Y == 4 && lastStart.Y == 6){
            if(pieces[board[p.X-1][p.Y].pieceID].type == 'p' && pieces[board[p.X-1][p.Y].pieceID].color == 0){
                temp.X = p.X-1;
                temp.Y = p.Y+1;
                legal.push_back(temp);
            }
        }
        else if(p.X+1 <= 7 && board[p.X+1][p.Y].X == lastEnd.X && board[p.X+1][p.Y].Y == 4 && lastStart.Y == 6){
            if(pieces[board[p.X+1][p.Y].pieceID].type == 'p' && pieces[board[p.X+1][p.Y].pieceID].color == 0){
                temp.X = p.X+1;
                temp.Y = p.Y+1;
                legal.push_back(temp);
            }
        }
    }
    break;

    //Rook
    case 'r':
    for(int i=1; i<8; i++){
        if(p.X+i > 7) break;
        else if(board[p.X+i][p.Y].exists && pieces[board[p.X+i][p.Y].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y;
            legal.push_back(temp);
            if(board[p.X+i][p.Y].exists) break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X-i < 0) break;
        else if(board[p.X-i][p.Y].exists && pieces[board[p.X-i][p.Y].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y;
            legal.push_back(temp);
            if(board[p.X-i][p.Y].exists) break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.Y+i > 7) break;
        else if(board[p.X][p.Y+i].exists && pieces[board[p.X][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X][p.Y+i].exists) break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.Y-i < 0) break;
        else if(board[p.X][p.Y-i].exists && pieces[board[p.X][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X][p.Y-i].exists) break;
        }
    }
    break;

    //Bishop
    case 'b':
    for(int i=1; i<8; i++){
        if(p.X+i > 7 || p.Y+i > 7) break;
        else if(board[p.X+i][p.Y+i].exists && pieces[board[p.X+i][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X+i][p.Y+i].exists)break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X-i < 0 || p.Y-i < 0) break;
        else if(board[p.X-i][p.Y-i].exists && pieces[board[p.X-i][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X-i][p.Y-i].exists)break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X-i < 0 || p.Y+i > 7) break;
        else if(board[p.X-i][p.Y+i].exists && pieces[board[p.X-i][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X-i][p.Y+i].exists)break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X+i > 7 || p.Y-i < 0) break;
        else if(board[p.X+i][p.Y-i].exists && pieces[board[p.X+i][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X+i][p.Y-i].exists)break;
        }
    }
    break;

    //Queen
    case 'q':
    for(int i=1; i<8; i++){
        if(p.X+i > 7) break;
        else if(board[p.X+i][p.Y].exists && pieces[board[p.X+i][p.Y].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y;
            legal.push_back(temp);
            if(board[p.X+i][p.Y].exists) break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X-i < 0) break;
        else if(board[p.X-i][p.Y].exists && pieces[board[p.X-i][p.Y].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y;
            legal.push_back(temp);
            if(board[p.X-i][p.Y].exists) break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.Y+i > 7) break;
        else if(board[p.X][p.Y+i].exists && pieces[board[p.X][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X][p.Y+i].exists) break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.Y-i < 0) break;
        else if(board[p.X][p.Y-i].exists && pieces[board[p.X][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X][p.Y-i].exists) break;
        }
    }

    for(int i=1; i<8; i++){
        if(p.X+i > 7 || p.Y+i > 7) break;
        else if(board[p.X+i][p.Y+i].exists && pieces[board[p.X+i][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X+i][p.Y+i].exists)break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X-i < 0 || p.Y-i < 0) break;
        else if(board[p.X-i][p.Y-i].exists && pieces[board[p.X-i][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X-i][p.Y-i].exists)break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X-i < 0 || p.Y+i > 7) break;
        else if(board[p.X-i][p.Y+i].exists && pieces[board[p.X-i][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X-i][p.Y+i].exists)break;
        }
    }
    for(int i=1; i<8; i++){
        if(p.X+i > 7 || p.Y-i < 0) break;
        else if(board[p.X+i][p.Y-i].exists && pieces[board[p.X+i][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X+i][p.Y-i].exists)break;
        }
    }
    break;

    //King
    case 'k':
    for(int i=1; i<2; i++){
        if(p.X+i > 7) break;
        else if(board[p.X+i][p.Y].exists && pieces[board[p.X+i][p.Y].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y;
            legal.push_back(temp);
            if(board[p.X+i][p.Y].exists) break;
        }
    }
    for(int i=1; i<2; i++){
        if(p.X-i < 0) break;
        else if(board[p.X-i][p.Y].exists && pieces[board[p.X-i][p.Y].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y;
            legal.push_back(temp);
            if(board[p.X-i][p.Y].exists) break;
        }
    }
    for(int i=1; i<2; i++){
        if(p.Y+i > 7) break;
        else if(board[p.X][p.Y+i].exists && pieces[board[p.X][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X][p.Y+i].exists) break;
        }
    }
    for(int i=1; i<2; i++){
        if(p.Y-i < 0) break;
        else if(board[p.X][p.Y-i].exists && pieces[board[p.X][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X][p.Y-i].exists) break;
        }
    }

    for(int i=1; i<2; i++){
        if(p.X+i > 7 || p.Y+i > 7) break;
        else if(board[p.X+i][p.Y+i].exists && pieces[board[p.X+i][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X+i][p.Y+i].exists)break;
        }
    }
    for(int i=1; i<2; i++){
        if(p.X-i < 0 || p.Y-i < 0) break;
        else if(board[p.X-i][p.Y-i].exists && pieces[board[p.X-i][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X-i][p.Y-i].exists)break;
        }
    }
    for(int i=1; i<2; i++){
        if(p.X-i < 0 || p.Y+i > 7) break;
        else if(board[p.X-i][p.Y+i].exists && pieces[board[p.X-i][p.Y+i].pieceID].color == p.color) break;
        else{
            temp.X = p.X-i;
            temp.Y = p.Y+i;
            legal.push_back(temp);
            if(board[p.X-i][p.Y+i].exists)break;
        }
    }
    for(int i=1; i<2; i++){
        if(p.X+i > 7 || p.Y-i < 0) break;
        else if(board[p.X+i][p.Y-i].exists && pieces[board[p.X+i][p.Y-i].pieceID].color == p.color) break;
        else{
            temp.X = p.X+i;
            temp.Y = p.Y-i;
            legal.push_back(temp);
            if(board[p.X+i][p.Y-i].exists)break;
        }
    }

    //Castling
    if(!p.hasMoved){
        if(board[0][p.Y].exists && !pieces[board[0][p.Y].pieceID].hasMoved){
            bool isBlocked = false;
            for(int i=1; i<=3; i++){
                if(board[i][p.Y].exists || isThreatened({i,p.Y}, p.color)){
                    isBlocked = true;
                    break;
                }
            }
            if(isThreatened({0,p.Y}, p.color) || isThreatened({4,p.Y}, p.color)) isBlocked = true;

            if(!isBlocked){
              temp.X = 2;
              temp.Y = p.Y;
              legal.push_back(temp);
            }
        }
    }
    if(!p.hasMoved){
        if(board[7][p.Y].exists && !pieces[board[7][p.Y].pieceID].hasMoved){
            bool isBlocked = false;
            for(int i=5; i<=6; i++){
                if(board[i][p.Y].exists || isThreatened({i,p.Y}, p.color)){
                    isBlocked = true;
                    break;
                }
            }
            if(isThreatened({7,p.Y}, p.color) || isThreatened({4,p.Y}, p.color)) isBlocked = true;

            if(!isBlocked){
              temp.X = 6;
              temp.Y = p.Y;
              legal.push_back(temp);
            }
        }
    }
    break;

    //Knight
    case 'n':
    if(p.X+2 <= 7 && p.Y+1 <= 7 && (!board[p.X+2][p.Y+1].exists || pieces[board[p.X+2][p.Y+1].pieceID].color != p.color)){
        temp.X = p.X+2;
        temp.Y = p.Y+1;
        legal.push_back(temp);
    }
    if(p.X+2 <= 7 && p.Y-1 >= 0 && (!board[p.X+2][p.Y-1].exists || pieces[board[p.X+2][p.Y-1].pieceID].color != p.color)){
        temp.X = p.X+2;
        temp.Y = p.Y-1;
        legal.push_back(temp);
    }
    if(p.X-2 >= 0 && p.Y+1 <= 7 && (!board[p.X-2][p.Y+1].exists || pieces[board[p.X-2][p.Y+1].pieceID].color != p.color)){
        temp.X = p.X-2;
        temp.Y = p.Y+1;
        legal.push_back(temp);
    }
    if(p.X-2 >= 0 && p.Y-1 >= 0 && (!board[p.X-2][p.Y-1].exists || pieces[board[p.X-2][p.Y-1].pieceID].color != p.color)){
        temp.X = p.X-2;
        temp.Y = p.Y-1;
        legal.push_back(temp);
    }
    
    if(p.X+1 <= 7 && p.Y+2 <= 7 && (!board[p.X+1][p.Y+2].exists || pieces[board[p.X+1][p.Y+2].pieceID].color != p.color)){
        temp.X = p.X+1;
        temp.Y = p.Y+2;
        legal.push_back(temp);
    }
    if(p.X+1 <= 7 && p.Y-2 >= 0 && (!board[p.X+1][p.Y-2].exists || pieces[board[p.X+1][p.Y-2].pieceID].color != p.color)){
        temp.X = p.X+1;
        temp.Y = p.Y-2;
        legal.push_back(temp);
    }
    if(p.X-1 >= 0 && p.Y+2 <= 7 && (!board[p.X-1][p.Y+2].exists || pieces[board[p.X-1][p.Y+2].pieceID].color != p.color)){
        temp.X = p.X-1;
        temp.Y = p.Y+2;
        legal.push_back(temp);
    }
    if(p.X-1 >= 0 && p.Y-2 >= 0 && (!board[p.X-1][p.Y-2].exists || pieces[board[p.X-1][p.Y-2].pieceID].color != p.color)){
        temp.X = p.X-1;
        temp.Y = p.Y-2;
        legal.push_back(temp);
    }
    break;

    default:
    break;
  }
  
  return checkChecks(p, legal);
}

bool Window::isThreatened(Coord target, int color){
  for(int i=1; i<8; i++){
      if(target.X+i > 7) break;
      else if(board[target.X+i][target.Y].exists && pieces[board[target.X+i][target.Y].pieceID].color == color) break;
      else if(board[target.X+i][target.Y].exists){
          if(pieces[board[target.X+i][target.Y].pieceID].type == 'r' ||
              pieces[board[target.X+i][target.Y].pieceID].type == 'q' ||
              (pieces[board[target.X+i][target.Y].pieceID].type == 'k' && i==1)) return true;
          else break;
      }
  }
  for(int i=1; i<8; i++){
      if(target.X-i < 0) break;
      else if(board[target.X-i][target.Y].exists && pieces[board[target.X-i][target.Y].pieceID].color == color) break;
      else if(board[target.X-i][target.Y].exists){
          if(pieces[board[target.X-i][target.Y].pieceID].type == 'r' ||
              pieces[board[target.X-i][target.Y].pieceID].type == 'q' ||
              (pieces[board[target.X-i][target.Y].pieceID].type == 'k' && i==1)) return true;
          else break;
      }
  }
  for(int i=1; i<8; i++){
      if(target.Y+i > 7) break;
      else if(board[target.X][target.Y+i].exists && pieces[board[target.X][target.Y+i].pieceID].color == color) break;
      else if(board[target.X][target.Y+i].exists){
          if(pieces[board[target.X][target.Y+i].pieceID].type == 'r' ||
              pieces[board[target.X][target.Y+i].pieceID].type == 'q' ||
              (pieces[board[target.X][target.Y+i].pieceID].type == 'k' && i==1)) return true;
          else break;
      }
  }
  for(int i=1; i<8; i++){
      if(target.Y-i < 0) break;
      else if(board[target.X][target.Y-i].exists && pieces[board[target.X][target.Y-i].pieceID].color == color) break;
      else if(board[target.X][target.Y-i].exists){
          if(pieces[board[target.X][target.Y-i].pieceID].type == 'r' ||
              pieces[board[target.X][target.Y-i].pieceID].type == 'q' ||
              (pieces[board[target.X][target.Y-i].pieceID].type == 'k' && i==1)) return true;
          else break;
      }
  }

  for(int i=1; i<8; i++){
      if(target.X+i > 7 || target.Y+i > 7) break;
      else if(board[target.X+i][target.Y+i].exists && pieces[board[target.X+i][target.Y+i].pieceID].color == color) break;
      else if(board[target.X+i][target.Y+i].exists){
          if(pieces[board[target.X+i][target.Y+i].pieceID].type == 'b' ||
              pieces[board[target.X+i][target.Y+i].pieceID].type == 'q' ||
              (pieces[board[target.X+i][target.Y+i].pieceID].type == 'k' && i==1) ||
              (pieces[board[target.X+i][target.Y+i].pieceID].type == 'p' && color == 1 && i==1)) return true;
          else break;
      }
  }
  for(int i=1; i<8; i++){
      if(target.X-i < 0 || target.Y-i < 0) break;
      else if(board[target.X-i][target.Y-i].exists && pieces[board[target.X-i][target.Y-i].pieceID].color == color) break;
      else if(board[target.X-i][target.Y-i].exists){
          if(pieces[board[target.X-i][target.Y-i].pieceID].type == 'b' ||
              pieces[board[target.X-i][target.Y-i].pieceID].type == 'q' ||
              (pieces[board[target.X-i][target.Y-i].pieceID].type == 'k' && i==1) ||
              (pieces[board[target.X-i][target.Y-i].pieceID].type == 'p' && color == 0 && i==1)) return true;
          else break;
      }
  }
  for(int i=1; i<8; i++){
      if(target.X-i < 0 || target.Y+i > 7) break;
      else if(board[target.X-i][target.Y+i].exists && pieces[board[target.X-i][target.Y+i].pieceID].color == color) break;
      else if(board[target.X-i][target.Y+i].exists){
          if(pieces[board[target.X-i][target.Y+i].pieceID].type == 'b' ||
              pieces[board[target.X-i][target.Y+i].pieceID].type == 'q' ||
              (pieces[board[target.X-i][target.Y+i].pieceID].type == 'k' && i==1) ||
              (pieces[board[target.X-i][target.Y+i].pieceID].type == 'p' && color == 1 && i==1)) return true;
          else break;
      }
  }
  for(int i=1; i<8; i++){
      if(target.X+i > 7 || target.Y-i < 0) break;
      else if(board[target.X+i][target.Y-i].exists && pieces[board[target.X+i][target.Y-i].pieceID].color == color) break;
      else if(board[target.X+i][target.Y-i].exists){
          if(pieces[board[target.X+i][target.Y-i].pieceID].type == 'b' ||
              pieces[board[target.X+i][target.Y-i].pieceID].type == 'q' ||
              (pieces[board[target.X+i][target.Y-i].pieceID].type == 'k' && i==1) ||
              (pieces[board[target.X+i][target.Y-i].pieceID].type == 'p' && color == 0 && i==1)) return true;
          else break;
      }
  }

  
  if(target.X+2 <= 7 && target.Y+1 <= 7 && (board[target.X+2][target.Y+1].exists && pieces[board[target.X+2][target.Y+1].pieceID].color != color && pieces[board[target.X+2][target.Y+1].pieceID].type == 'n')){
      return true;
  }
  if(target.X+2 <= 7 && target.Y-1 >= 0 && (board[target.X+2][target.Y-1].exists && pieces[board[target.X+2][target.Y-1].pieceID].color != color && pieces[board[target.X+2][target.Y-1].pieceID].type == 'n')){
      return true;
  }
  if(target.X-2 >= 0 && target.Y+1 <= 7 && (board[target.X-2][target.Y+1].exists && pieces[board[target.X-2][target.Y+1].pieceID].color != color && pieces[board[target.X-2][target.Y+1].pieceID].type == 'n')){
      return true;
  }
  if(target.X-2 >= 0 && target.Y-1 >= 0 && (board[target.X-2][target.Y-1].exists && pieces[board[target.X-2][target.Y-1].pieceID].color != color && pieces[board[target.X-2][target.Y-1].pieceID].type == 'n')){
      return true;
  }
  
  if(target.X+1 <= 7 && target.Y+2 <= 7 && (board[target.X+1][target.Y+2].exists && pieces[board[target.X+1][target.Y+2].pieceID].color != color && pieces[board[target.X+1][target.Y+2].pieceID].type == 'n')){
      return true;
  }
  if(target.X+1 <= 7 && target.Y-2 >= 0 && (board[target.X+1][target.Y-2].exists && pieces[board[target.X+1][target.Y-2].pieceID].color != color && pieces[board[target.X+1][target.Y-2].pieceID].type == 'n')){
      return true;
  }
  if(target.X-1 >= 0 && target.Y+2 <= 7 && (board[target.X-1][target.Y+2].exists && pieces[board[target.X-1][target.Y+2].pieceID].color != color && pieces[board[target.X-1][target.Y+2].pieceID].type == 'n')){
      return true;
  }
  if(target.X-1 >= 0 && target.Y-2 >= 0 && (board[target.X-1][target.Y-2].exists && pieces[board[target.X-1][target.Y-2].pieceID].color != color && pieces[board[target.X-1][target.Y-2].pieceID].type == 'n')){
      return true;
  }

return false;
}

std::vector<Coord> Window::checkChecks(Piece p, std::vector<Coord> candidates){
  Tile buffer;
  bool markForRemoval = false;
  for(int i=candidates.size()-1; i>=0; i--){
    buffer = board[candidates[i].X][candidates[i].Y];
    board[candidates[i].X][candidates[i].Y].exists = true;
    board[candidates[i].X][candidates[i].Y].pieceID = movingPieceID;
    board[pieces[movingPieceID].X][pieces[movingPieceID].Y].exists = false;
    board[pieces[movingPieceID].X][pieces[movingPieceID].Y].pieceID = -1;

    if(pieces[movingPieceID].type == 'k') markForRemoval = isThreatened({candidates[i].X, candidates[i].Y}, p.color);
    else markForRemoval = isThreatened(kingPos[p.color], p.color);

    board[pieces[movingPieceID].X][pieces[movingPieceID].Y].exists = true;
    board[pieces[movingPieceID].X][pieces[movingPieceID].Y].pieceID = movingPieceID;
    board[candidates[i].X][candidates[i].Y] = buffer;

    if(markForRemoval) candidates.erase(candidates.begin() + i);
    markForRemoval = false;
  }

  return candidates;
}

bool Window::checkForGameOver(){
  for(int i=0;i<pieces.size();i++){
    if(pieces[i].color == turn && !pieces[i].captured && pieces[i].legalMoves.size() > 0) return false;
  }
  return true;  
}


void Window::renderModel(Model m, int color, glm::vec3 pos){
  // Use currently selected program
  auto const program{m_programs.at(m_currentProgramIndex)};
  abcg::glUseProgram(program);

  auto const modelMatrixLoc{abcg::glGetUniformLocation(program, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(program, "normalMatrix")};
  auto const shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  auto const KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(program, "Ks")};

  auto const modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glm::mat4 model{1.0f};
  switch(color)
  {
    //Pieces 0=White 1=Black
    case 0:
    //abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); 
    m_Ka.r = 0.473;
    m_Ka.g = 0.380;
    m_Ka.b = 0.345;

    m_Kd.r = 0.848;
    m_Kd.g = 0.786;
    m_Kd.b = 0.762;

    m_Ks.r = 1.000;
    m_Ks.g = 1.000;
    m_Ks.b = 1.000;
    break;
    case 1:
    //abcg::glUniform4f(colorLoc, 0.1f, 0.1f, 0.1f, 1.0f);
    m_Ka.r = 0.059;
    m_Ka.g = 0.058;
    m_Ka.b = 0.067;

    m_Kd.r = 0.134;
    m_Kd.g = 0.140;
    m_Kd.b = 0.181;

    m_Ks.r = 0.670;
    m_Ks.g = 0.686;
    m_Ks.b = 0.878;
    break;

    //Tiles 2=White 3=Black
    case 2:
    //abcg::glUniform4f(colorLoc, 0.93f, 0.86f, 0.59f, 1.0f); 
    m_Ka.r = 0.405;
    m_Ka.g = 0.293;
    m_Ka.b = 0.166;

    m_Kd.r = 0.574;
    m_Kd.g = 0.491;
    m_Kd.b = 0.288;

    m_Ks.r = 0.759;
    m_Ks.g = 0.673;
    m_Ks.b = 0.487;
    break;
    case 3:
    //abcg::glUniform4f(colorLoc, 0.35f, 0.3f, 0.13f, 1.0f);
    m_Ka.r = 0.122;
    m_Ka.g = 0.071;
    m_Ka.b = 0.033;

    m_Kd.r = 0.295;
    m_Kd.g = 0.190;
    m_Kd.b = 0.082;

    m_Ks.r = 0.616;
    m_Ks.g = 0.471;
    m_Ks.b = 0.289;
    break;

    //Selected Tile 4
    case 4:
    //abcg::glUniform4f(colorLoc, 1.0f, 0.5f, 0.5f, 1.0f); 
    m_Ka.r = 0.600;
    m_Ka.g = 0.193;
    m_Ka.b = 0.066;

    m_Kd.r = 1.000;
    m_Kd.g = 0.391;
    m_Kd.b = 0.188;

    m_Ks.r = 1.000;
    m_Ks.g = 0.573;
    m_Ks.b = 0.387;
    break;

    //Last Move 5=White 6=Black
    case 5:
    //abcg::glUniform4f(colorLoc, 0.75f, 0.75f, 1.0f, 1.0f); 
    m_Ka.r = 0.405;
    m_Ka.g = 0.293;
    m_Ka.b = 0.166;

    m_Kd.r = 0.574;
    m_Kd.g = 0.491;
    m_Kd.b = 0.788;

    m_Ks.r = 0.759;
    m_Ks.g = 0.673;
    m_Ks.b = 0.487;
    break;
    case 6:
    //abcg::glUniform4f(colorLoc, 0.375f, 0.375f, 0.5f, 1.0f);
    m_Ka.r = 0.122;
    m_Ka.g = 0.071;
    m_Ka.b = 0.033;

    m_Kd.r = 0.295;
    m_Kd.g = 0.190;
    m_Kd.b = 0.582;

    m_Ks.r = 0.616;
    m_Ks.g = 0.471;
    m_Ks.b = 0.289;
    break;

    //Legal Move 5=White 6=Black
    case 7:
    //abcg::glUniform4f(colorLoc, 0.75f, 1.0f, 0.75f, 1.0f); 
    m_Ka.r = 0.405;
    m_Ka.g = 0.293;
    m_Ka.b = 0.166;

    m_Kd.r = 0.574;
    m_Kd.g = 0.991;
    m_Kd.b = 0.288;

    m_Ks.r = 0.759;
    m_Ks.g = 0.673;
    m_Ks.b = 0.487;
    break;
    case 8:
    //abcg::glUniform4f(colorLoc, 0.375f, 0.5f, 0.375f, 1.0f);
    m_Ka.r = 0.122;
    m_Ka.g = 0.071;
    m_Ka.b = 0.033;

    m_Kd.r = 0.295;
    m_Kd.g = 0.690;
    m_Kd.b = 0.082;

    m_Ks.r = 0.616;
    m_Ks.g = 0.471;
    m_Ks.b = 0.289;
    break;
    //Checked King 9=White 10=Black
    case 9:
    m_Ka.r = 1.000;
    m_Ka.g = 0.280;
    m_Ka.b = 0.245;

    m_Kd.r = 1.000;
    m_Kd.g = 0.686;
    m_Kd.b = 0.662;

    m_Ks.r = 1.000;
    m_Ks.g = 1.000;
    m_Ks.b = 1.000;
    break;
    case 10:
    m_Ka.r = 0.209;
    m_Ka.g = 0.008;
    m_Ka.b = 0.017;

    m_Kd.r = 0.284;
    m_Kd.g = 0.090;
    m_Kd.b = 0.131;

    m_Ks.r = 1.000;
    m_Ks.g = 0.686;
    m_Ks.b = 0.878;
    break;
  }
    
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

  model = glm::translate(model, pos);
  model = glm::scale(model, glm::vec3(0.03f));
  if(color == 0) model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 1, 0));
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);

  m.render();
}

void Window::renderBoard(){
  for(int i=0;i<8;i++)
    for(int j=0;j<8;j++){
      if(selectedX==i && selectedY==j){
        renderModel(m_tile,4,board[i][j].pos);
        continue;
      }
      if(movingPiece){
        for(int k=0; k<pieces[movingPieceID].legalMoves.size(); k++){
          if(pieces[movingPieceID].legalMoves[k].X==i && pieces[movingPieceID].legalMoves[k].Y==j){
            renderModel(m_tile,board[i][j].color + 5,board[i][j].pos);
          }
        }
      }
      if((lastStart.X==i && lastStart.Y==j) || (lastEnd.X==i && lastEnd.Y==j)){
        renderModel(m_tile,board[i][j].color + 3,board[i][j].pos);
        continue;
      }
      renderModel(m_tile,board[i][j].color,board[i][j].pos);
    }
}

void Window::renderPieces(){
  for(int i=0;i<pieces.size();i++){
    if(!pieces[i].captured){
      switch(pieces[i].type){
      case 'p':
      renderModel(m_pawn,pieces[i].color,pieces[i].pos);
      break;
      case 'r':
      renderModel(m_rook,pieces[i].color,pieces[i].pos);
      break;
      case 'n':
      renderModel(m_knight,pieces[i].color,pieces[i].pos);
      break;
      case 'b':
      renderModel(m_bishop,pieces[i].color,pieces[i].pos);
      break;
      case 'q':
      renderModel(m_queen,pieces[i].color,pieces[i].pos);
      break;
      case 'k':
      if((pieces[i].color == 0 && whiteKingChecked) || (pieces[i].color == 1 && blackKingChecked)) renderModel(m_king,pieces[i].color + 9,pieces[i].pos);
      else renderModel(m_king,pieces[i].color,pieces[i].pos);
      break;
    }}
  }
}

void Window::onDestroy() {
  m_pawn.destroy();
  m_rook.destroy();
  m_knight.destroy();
  m_bishop.destroy();
  m_queen.destroy();
  m_king.destroy();

  m_tile.destroy();

  for (auto const &program : m_programs) {
    abcg::glDeleteProgram(program);
  }
}
#include "funcoes.h"

extern Janela MainWindow;
extern Circulo arena[2];
extern Retangulo rect;
extern vector<Circulo> enemies;
extern Carro player;
extern list<Tiro> shoots;
extern int key_status[256];

void readXMLFile(const char *path)
{
    const char config_file_name[] = "config.xml";

    string svg_name, svg_type, svg_path, pathFile;
    XMLDocument config_file, svg_file;
    XMLError statusLoad;
    XMLElement *pElem;
    XMLNode *pRoot;

    if(!path) //Verify if the path was provided
    {
        cout << "Path to config file not provided, the program will be finished" << endl;
        exit(1);
    }

    pathFile = path;
    pathFile += config_file_name;
    //cout << pathFile << endl;

    //Opening file
    statusLoad = config_file.LoadFile(pathFile.c_str());

    if(statusLoad != XML_SUCCESS) //Test if file has been opened correctly
    {
        cerr << "Error opening configuration file\nProgram will be finished" << endl;
        exit(XML_ERROR_FILE_READ_ERROR);
    }

    pRoot = config_file.FirstChild(); //Get first element

    if(!pRoot)
    {
        cerr << "Error parsing element on configuration file\nProgram will be finished" << endl;
        exit(XML_ERROR_PARSING_ELEMENT);
    }
    //cout << pRoot->Value() << endl;

    //config.xml file

    //Arena info
    pElem = pRoot->FirstChildElement("arquivoDaArena");
    svg_name = pElem->FindAttribute("nome")->Value();
    svg_type = pElem->FindAttribute("tipo")->Value();
    svg_path = pElem->FindAttribute("caminho")->Value();
    //    cout << name << endl;
    //    cout << type << endl;
    //    cout << svg_path << endl;

    //Car info
    pElem = pRoot->FirstChildElement("carro");
    player.setShotSpeed(pElem->FindAttribute("velTiro")->FloatValue());
    player.setCarSpeed(pElem->FindAttribute("velCarro")->FloatValue());

    // cout << "velTiro " << velTiro << endl;
    // cout << "velCarro " << velCarro << endl;

    //Path and name to svg file

    if(svg_path[0] == '~') //Testing if there is a tilde on beginning of path
    {
        pathFile = getenv("HOME");
        svg_path.erase(0,1);
        pathFile += svg_path;
        // cout << pathFile << endl;
    }
    else
    {
        pathFile = svg_path;
    }

    pathFile += svg_name += ".";
    pathFile += svg_type;

    // cout << pathFile << endl;

    //SVG file
    statusLoad = svg_file.LoadFile(pathFile.c_str());

    if(statusLoad != XML_SUCCESS) //Test if file has been opened correctly
    {
        cerr << "Error opening svg file\nProgram will be finished" << endl;
        exit(XML_ERROR_FILE_READ_ERROR);
    }

    pRoot = svg_file.FirstChild(); //Get first element

    if(!pRoot)
    {
        cerr << "Error parsing element on svg file\nProgram will be finished" << endl;
        exit(XML_ERROR_PARSING_ELEMENT);
    }
//    cout << pRoot->Value() << endl;

    pElem = pRoot->FirstChildElement();
    // cout << pElem->Value() << endl;

    for(; pElem != NULL; pElem = pElem->NextSiblingElement())
    {
        string tipo = pElem->Value();

        if(!tipo.compare("circle")) //If is a circle
        {
            Circulo c;
            c.setXc(atof(pElem->FindAttribute("cx")->Value()));
            c.setYc(atof(pElem->FindAttribute("cy")->Value()));
            c.setRadius(atof(pElem->FindAttribute("r")->Value()));
            c.setId(pElem->FindAttribute("id")->Value());
            c.setRGBColors(pElem->FindAttribute("fill")->Value());

            // cout << pElem->FindAttribute("cx")->Value() << endl;
            // cout << pElem->FindAttribute("cy")->Value() << endl;
            // cout << pElem->FindAttribute("r")->Value() << endl;
            // cout << pElem->FindAttribute("id")->Value() << endl;
            // cout << pElem->FindAttribute("fill")->Value() << endl;

            //cout << "Circulo " << "cx: " << c.getXc() << " cy: " << c.getYc() << " r: " << c.getRadius() <<
            //" colors: " << c.getRGBColors(RED) << ", " << c.getRGBColors(GREEN) << ", " << c.getRGBColors(BLUE) << endl;
            if(!c.getId().compare("Jogador"))
            {
                player.copyInfo(c);
            }
            else if(!c.getId().compare("Pista"))
                {
                    if(arena[0].getRadius() > c.getRadius())
                        arena[1] = c;
                    else{
                            arena[1] = arena[0];
                            arena[0] = c;
                        }
                }
                else enemies.push_back(c);
        }
        else if(!tipo.compare("rect")) //If is a rectangle
            {
                rect.setXc(atof(pElem->FindAttribute("x")->Value()));
                rect.setYc(atof(pElem->FindAttribute("y")->Value()));
                rect.setWidth(atof(pElem->FindAttribute("width")->Value()));
                rect.setHeight(atof(pElem->FindAttribute("height")->Value()));
                rect.setId(pElem->FindAttribute("id")->Value());
                rect.setRGBColors(pElem->FindAttribute("fill")->Value());

                // cout << pElem->FindAttribute("cx")->Value() << endl;
                // cout << pElem->FindAttribute("cy")->Value() << endl;
                // cout << pElem->FindAttribute("r")->Value() << endl;
                // cout << pElem->FindAttribute("id")->Value() << endl;
                // cout << pElem->FindAttribute("fill")->Value() << endl;

                // cout << "Retangulo " << "x: " << rect.getXc() << " y: " << rect.getYc() <<
                // " width: " << rect.getWidth() << " height: " << rect.getHeight() << " colors: " <<
                // rect.getRGBColors(RED) << ", " << rect.getRGBColors(GREEN) << ", " << rect.getRGBColors(BLUE) << endl;
            }
    } //End SVG file

    MainWindow.setHeight(2*arena[0].getRadius());
    MainWindow.setWidth(2*arena[0].getRadius());
    MainWindow.setTitle("Arena");

    //Adjusting Y-Axis -> y = hy - y;  and Change center coordinate to (R,R)

    //Calculating distance to new center
    float dx = arena[0].getXc() - arena[0].getRadius(), dy = arena[0].getYc() - arena[0].getRadius();

    //Changing center to coordinate (Radius, Radius)
    for(int i = 0; i < 2; i++)
    {
        arena[i].setXc(arena[0].getRadius());
        arena[i].setYc(arena[0].getRadius());
    }

    rect.setYc(rect.getYc() - dy);
    rect.setXc(rect.getXc() - dx);
    rect.setYc(MainWindow.getHeight() - rect.getYc());
    rect.updateVertices();
    // cout << "x: " << rect.getXc() << "y: " << rect.getYc() << endl;

    for(vector<Circulo>::iterator it = enemies.begin(); it != enemies.end(); it++)
    {
        (*it).setXc((*it).getXc() - dx);
        (*it).setYc((*it).getYc() - dy);
        (*it).setYc(MainWindow.getHeight() - (*it).getYc());
    }

    player.setXc(player.getXc() - dx);
    player.setYc(player.getYc() - dy);
    player.setYc(MainWindow.getHeight() - player.getYc());

    // for(vector<Circulo>::iterator it = enemies.begin(); it != enemies.end(); it++)
    // cout << (*it).getId() << endl;

    return;
}

//OpenGL functions

void drawRectangle(float x1, float y1, float x2, float y2, const float colors[3])
{
    glColor3fv((GLfloat*)(colors));
    glBegin(GL_POLYGON);
        glVertex3f(x1, y1, 0.0);
        glVertex3f(x1, y2, 0.0);
        glVertex3f(x2, y2, 0.0);
        glVertex3f(x2, y1, 0.0);
    glEnd();
}

void drawCircle(float xc, float yc, float radius, const float colors[3], int resolution)
{
    float dx, dy;
    glColor3fv((GLfloat*)(colors));
    glBegin(GL_TRIANGLE_FAN);
		glVertex2f(xc, yc);
		for(int i = 0; i <= resolution; i++)
        {
            dx = xc + (radius * cos(i * 2.0*M_PI / resolution));
            dy = yc + (radius * sin(i * 2.0*M_PI / resolution));
			glVertex2f(dx, dy);
		}
    glEnd();
}

void drawEllipse(float xc, float yc, float width, float height, const float colors[3], int resolution)
{
    float dx, dy;
    glColor3fv((GLfloat*)(colors));
    glBegin(GL_TRIANGLE_FAN);
		glVertex2f(xc, yc);
		for(int i = 0; i <= resolution; i++)
        {
            dx = xc + (width * cos(i * 2.0*M_PI / resolution));
            dy = yc + (height * sin(i * 2.0*M_PI / resolution));
			glVertex2f(dx, dy);
		}
    glEnd();
}

void init(void)
{
    /*Selecting background color*/
    //cout << " Janela "<< MainWindow.getBgColors(RED) << MainWindow.getBgColors(GREEN) << MainWindow.getBgColors(BLUE) << endl;
    glClearColor(MainWindow.getBgColors(RED),MainWindow.getBgColors(GREEN),MainWindow.getBgColors(BLUE), 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 2*arena[0].getRadius(), 0.0, 2*arena[0].getRadius(), -1.0, 1.0);
}

void display(void)
{
    /*Cleaning pixels */
    glClear(GL_COLOR_BUFFER_BIT);

    for(int i=0; i < 2; i++)
        arena[i].draw();

    rect.draw();

    for(vector<Circulo>::iterator it = enemies.begin(); it != enemies.end(); it++)
        (*it).draw();

    player.draw();

    for(list<Tiro>::iterator it = shoots.begin(); it != shoots.end(); it++)
        (*it).draw();

    glutSwapBuffers();
}

void idle(void)
{
    float tx, ty;
    const float WHEEL_ROTATION_STEP = 1;
    float wheelTheta = player.getWheelRotation();
    float move_vector[3] = {0}, *p = NULL;

    static GLdouble previousTime = 0;
    GLdouble currentTime;
    GLdouble timeDiference;

    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDiference = currentTime - previousTime; // Elapsed time from the previous frame.
    previousTime = currentTime; //Update previous time

    if(key_status['w'])
    {
        p = player.move(true, timeDiference);
        move_vector[X_AXIS] = p[X_AXIS];
        move_vector[Y_AXIS] = p[Y_AXIS];
    }
    if(key_status['s'])
    {
        p = player.move(false, timeDiference);
        move_vector[X_AXIS] = p[X_AXIS];
        move_vector[Y_AXIS] = p[Y_AXIS];
    }
    if(key_status['d'])
    {
        wheelTheta -= WHEEL_ROTATION_STEP;
        player.setWheelRotation(wheelTheta);
    }
    if(key_status['a'])
    {
        wheelTheta += WHEEL_ROTATION_STEP;
        player.setWheelRotation(wheelTheta);
    }

    // Collision verification
    tx = player.getXc();
    ty = player.getYc();

    bool teste = true;

    // Test new position x
    player.setXc(tx + move_vector[X_AXIS]);

    for(vector<Circulo>::iterator it = enemies.begin(); it != enemies.end(); it++)
    {
        teste = teste && (*it).outsideCircle(player);
    }

    if(arena[0].insideCircle(player) && arena[1].outsideCircle(player) && teste);
    else
    {
        player.setXc(tx);
    }

    // Test new position y
    player.setYc(ty + move_vector[Y_AXIS]);

    for(vector<Circulo>::iterator it = enemies.begin(); it != enemies.end(); it++)
    {
        teste = teste && (*it).outsideCircle(player);
    }

    if(arena[0].insideCircle(player) && arena[1].outsideCircle(player) && teste);
    else
    {
        player.setYc(ty);
    }

    // End collision verification

    //Shot moving
    for(list<Tiro>::iterator it = shoots.begin(); it != shoots.end(); it++)
    {
        (*it).move(timeDiference);

        if(!(*it).isInWindow(0.0, 0.0, 2*arena[0].getRadius(), 2*arena[0].getRadius()))
        {
            it = shoots.erase(it);
            if(shoots.empty())
                break;
        }

    }
//    cout << shoots.size() << endl;

    glutPostRedisplay();
}

void keyUp (unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
        case 'W':
            key_status['w'] = 0;
            player.setMoving(false);
          break;

        case 's':
        case 'S':
            key_status['s'] = 0;
            player.setMoving(false);
          break;

        case 'd':
        case 'D':
          key_status['d'] = 0;
          break;

        case 'a':
        case 'A':
          key_status['a'] = 0;
          break;

        default:
            break;
      }
}

void keypress (unsigned char key, int x, int y)
{
  switch (key)
  {
      case 'w':
      case 'W':
          key_status['w'] = 1;
          player.setMoving(true);
        break;

      case 's':
      case 'S':
          key_status['s'] = 1;
          player.setMoving(true);
        break;

      case 'd':
      case 'D':
        key_status['d'] = 1;
        break;

      case 'a':
      case 'A':
        key_status['a'] = 1;
        break;

       case 'e':
         exit(1);

      default:
          break;
  }
}

void mouse(int key, int state, int x, int y)
{
    int hy = MainWindow.getHeight();

    y = hy - y; //Adjusting Y-Axis

    if (key == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        int hx = MainWindow.getWidth();
        int theta = -120.0/hx*x + 60.0;

        player.setGunRotation(theta);

        Tiro t = player.shoot();
        shoots.push_back(t);
    }
}

void passiveMouse(int x, int y)
{
    int hx = MainWindow.getWidth();
    int theta = -120.0/hx*x + 60.0;

    player.setGunRotation(theta);

    // cout << player.getGunRotation() << endl;

}

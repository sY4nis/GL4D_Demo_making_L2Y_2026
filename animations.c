/*!\file animations.c
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date April 12, 2023
 */

#include <GL4D/gl4dh.h>
#include "audioHelper.h"
#include <assert.h>
#include <stdlib.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <SDL_image.h>

// DEMO
#include <limits.h>
#define ECHANTILLONS 1024
typedef struct{
  GLfloat x, y;
} vec2d_t ;

typedef struct {
  vec2d_t p, v;
  GLfloat couleur[4];
} gouttes_t;
// DEMO



/*!\brief identifiant de la géométrie QUAD GL4Dummies */
static GLuint _quadId = 0;

static void _load_texture(GLuint id, const char * filename) {
  SDL_Surface * t;
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  t = IMG_Load(filename);
  assert(t);
  SDL_Surface * c = SDL_CreateRGBSurface(0, t->w, t->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  SDL_BlitSurface(t, NULL /* NULL => tout */, c, NULL /* tout */);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, c->w, c->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, c->pixels);
  SDL_FreeSurface(c);
  SDL_FreeSurface(t);
}

void fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  int vp[4], i;
  GLint tId;
  static GLuint tex[2], pId;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    glGetIntegerv(GL_VIEWPORT, vp);
    glGenTextures(2, tex);
    for(i = 0; i < 2; i++) {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mix.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tex[0]) {
      glDeleteTextures(2, tex);
      tex[0] = tex[1] = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
    /* JOUER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[0],  0);
    if(a0) a0(state);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[1],  0);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tId,  0);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    if(et / (GLfloat)t > 1) {
      fprintf(stderr, "%d-%d -- %f\n", et, t, et / (GLfloat)t);
      exit(0);
    }
    glUniform1f(glGetUniformLocation(pId, "dt"), et / (GLfloat)t);
    glUniform1i(glGetUniformLocation(pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(pId, "tex1"), 1);
    gl4dgDraw(_quadId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
}

void fondui(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
  /* INITIALISEZ VOS VARIABLES */
  int vp[4], i;
  GLint tId;
  static GLuint tex[3], pId;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE TRANSITION (SES VARIABLES <STATIC>s) */
    glGetIntegerv(GL_VIEWPORT, vp);
    glGenTextures(3, tex);
    for(i = 0; i < 3; i++) {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    _load_texture(tex[2], "images/fondui.jpg");
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mixi.fs", NULL);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(tex[0]) {
      glDeleteTextures(3, tex);
      tex[0] = tex[1] = 0;
    }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
    if(a0) a0(state);
    if(a1) a1(state);
    return;
  default: /* GL4DH_DRAW */
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
    /* JOUER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[0],  0);
    if(a0) a0(state);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tex[1],  0);
    if(a1) a1(state);
    /* MIXER LES DEUX ANIMATIONS */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  tId,  0);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    if(et / (GLfloat)t > 1) {
      fprintf(stderr, "%d-%d -- %f\n", et, t, et / (GLfloat)t);
      exit(0);
    }
    glUniform1f(glGetUniformLocation(pId, "dt"), et / (GLfloat)t);
    glUniform1i(glGetUniformLocation(pId, "tex0"), 0);
    glUniform1i(glGetUniformLocation(pId, "tex1"), 1);
    glUniform1i(glGetUniformLocation(pId, "tex2"), 2);
    gl4dgDraw(_quadId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
}

void rouge(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}

void vert(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}

void bleu(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}

// DEMO
void titre(int state) {
  static GLuint tex[1], pId;
  switch(state) {
    case GL4DH_INIT:
    //generation d'une texture
    glGenTextures(1, tex);
    //charge l'image dans la texture généré
    _load_texture(tex[0], "images/titre.jpg");
    pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
      return;
    case GL4DH_FREE:
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      return;
    default: 
    glDisable(GL_DEPTH_TEST);
    glUseProgram(pId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glUniform1i(glGetUniformLocation(pId, "inv"), 1);
    gl4dgDraw(_quadId);
    return;
  }
}
void noir(int state) {
  switch(state) {
  case GL4DH_INIT:
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
}

void pluie(int state){
  static gouttes_t * gouttes = NULL;
  static int nb_gouttes = 500;
  static GLuint pId, drop; 
  static double t0 = 0;
  switch(state) {
    case GL4DH_INIT:
    drop = gl4dgGenQuadf();
    pId = gl4duCreateProgram("<vs>shaders/phy2D.vs", "<fs>shaders/phy2D.fs", NULL);
    gl4duGenMatrix(GL_FLOAT, "mod");
    gl4duBindMatrix("mod");
    gl4duLoadIdentityf();
    gl4duGenMatrix(GL_FLOAT, "view");
    gl4duBindMatrix("view");
    gl4duLoadIdentityf();
    gl4duGenMatrix(GL_FLOAT, "proj");
    gl4duBindMatrix("proj");
    gl4duLoadIdentityf();
    gl4duOrthof(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);
    gouttes = malloc(nb_gouttes * sizeof *gouttes);
    for(int i = 0; i< nb_gouttes; i++){
      gouttes[i].p.x =0.89f * gl4dmSURand();
      gouttes[i].p.y = gl4dmSURand();
      gouttes[i].v.x = 0.5f * gl4dmSURand();
      gouttes[i].v.y = -0.8f - 0.4f * gl4dmURand();
      gouttes[i].couleur[0] = 1.0f;
      gouttes[i].couleur[1] = 1.0f;
      gouttes[i].couleur[2] = 1.0f;
      gouttes[i].couleur[3] = 0.5f;
    }
    return;
  case GL4DH_FREE:
    free(gouttes);
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    int i, len = ahGetAudioStreamLength();
    Sint16 *s = (Sint16 *)ahGetAudioStream();
    float amplitude = 0;
    if(len >= 2 * ECHANTILLONS){
      for(i = 0; i < ECHANTILLONS; i++){
          amplitude += abs(s[i]);
      }
      amplitude /= 256 * (float)INT16_MAX;
    }
    double t = gl4dhGetTicks() / 1000.0, dt = t - t0;
    t0 = t;
    for(int j = 0; j< nb_gouttes;j++){
      gouttes[j].p.y += gouttes[j].v.y * dt * (1.0f + amplitude * 3.0f);
      if(gouttes[j].p.y <= -1.0f) {
          gouttes[j].p.y = 1.0f;
          gouttes[j].p.x = gl4dmSURand();
      }
    }
    return;
  default: /* GL4DH_DRAW */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(pId);
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duBindMatrix("mod");
  gl4duLoadIdentityf();
  for(int i = 0; i < nb_gouttes; i++) {
    gl4duPushMatrix();
    gl4duTranslatef(gouttes[i].p.x, gouttes[i].p.y, -1.0f);
    gl4duScalef(0.005f, 0.03f, 0.005f);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(pId, "color"), 1, gouttes[i].couleur);
    gl4dgDraw(drop);
    gl4duPopMatrix();
  }
  return;
  }
}
// DEMO

void animationsInit(void) {
  if(!_quadId)
    _quadId = gl4dgGenQuadf();
}


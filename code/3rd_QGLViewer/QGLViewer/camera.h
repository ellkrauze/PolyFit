#ifndef QGLVIEWER_CAMERA_H
#define QGLVIEWER_CAMERA_H

#include "../../basic/canvas.h"
#include <QMap>
#include "keyFrameInterpolator.h"
class QGLViewer;

namespace qglviewer {

class ManipulatedCameraFrame;

/*! \brief A perspective or orthographic camera.
  \class Camera camera.h QGLViewer/camera.h

  A Camera defines some intrinsic parameters (fieldOfView(), position(),
  viewDirection(), upVector()...) and useful positioning tools that ease its
  placement (showEntireScene(), fitSphere(), lookAt()...). It exports its
  associated OpenGL projection and modelview matrices and can interactively be
  modified using the mouse.

  <h3>Mouse manipulation</h3>

  The position() and orientation() of the Camera are defined by a
  ManipulatedCameraFrame (retrieved using frame()). These methods are just
  convenient wrappers to the equivalent Frame methods. This also means that the
  Camera frame() can be attached to a Frame::referenceFrame() which enables
  complex Camera setups.

  Different displacements can be performed using the mouse. The list of possible
  actions is defined by the QGLViewer::MouseAction enum. Use
  QGLViewer::setMouseBinding() to attach a specific action to an arbitrary mouse
  button-state key binding. These actions are detailed in the <a
  href="../mouse.html">mouse page</a>.

  The default button binding are: QGLViewer::ROTATE (left), QGLViewer::ZOOM
  (middle) and QGLViewer::TRANSLATE (right). With this configuration, the Camera
  \e observes a scene and rotates around its pivotPoint(). You can switch
  between this mode and a fly mode using the QGLViewer::CAMERA_MODE (see
  QGLViewer::toggleCameraMode()) keyboard shortcut (default is 'Space').

  <h3>Other functionalities</h3>

  The type() of the Camera can be Camera::ORTHOGRAPHIC or Camera::PERSPECTIVE
  (see Type()). fieldOfView() is meaningless with Camera::ORTHOGRAPHIC.

  The near and far planes of the Camera are fitted to the scene and determined
  from QGLViewer::sceneRadius(), QGLViewer::sceneCenter() and
  zClippingCoefficient() by the zNear() and zFar() methods. Reasonable values on
  the scene extends hence have to be provided to the QGLViewer in order for the
  Camera to correctly display the scene. High level positioning methods also use
  this information (showEntireScene(), centerScene()...).

  A Camera holds KeyFrameInterpolator that can be used to save Camera positions
  and paths. You can interactively addKeyFrameToPath() to a given path using the
  default \c Alt+F[1-12] shortcuts. Use playPath() to make the Camera follow the
  path (default shortcut is F[1-12]). See the <a
  href="../keyboard.html">keyboard page</a> for details on key customization.

  Use cameraCoordinatesOf() and worldCoordinatesOf() to convert to and from the
  Camera frame() coordinate system. projectedCoordinatesOf() and
  unprojectedCoordinatesOf() will convert from screen to 3D coordinates.
  convertClickToLine() is very useful for analytical object selection.

  Stereo display is possible on machines with quad buffer capabilities (with
  Camera::PERSPECTIVE type() only). Test the <a
  href="../examples/stereoViewer.html">stereoViewer example</a> to check.

  A Camera can also be used outside of a QGLViewer or even without OpenGL for
  its coordinate system conversion capabilities. Note however that some of them
  explicitly rely on the presence of a Z-buffer. \nosubgrouping */
class QGLVIEWER_EXPORT Camera : public QObject, public CameraBase{
#ifndef DOXYGEN
  friend class ::QGLViewer;
#endif

  Q_OBJECT

public:
  Camera();
  virtual ~Camera();

  Camera(const Camera &camera);
  Camera &operator=(const Camera &camera);

  /*! Enumerates the two possible types of Camera.

  See type() and setType(). This type mainly defines different Camera projection
  matrix (see loadProjectionMatrix()). Many other methods (pointUnderPixel(),
  convertClickToLine(), projectedCoordinatesOf(), pixelGLRatio()...) are
  affected by this Type. */
  enum Type { PERSPECTIVE, ORTHOGRAPHIC };

  /*! @name Position and orientation */
  //@{
public:
  Vec position() const;
  Vec upVector() const;
  Vec viewDirection() const;
  Vec rightVector() const;
  Quaternion orientation() const;

  void setFromModelViewMatrix(const GLdouble *const modelViewMatrix);
  void setFromProjectionMatrix(const qreal matrix[12]);

public Q_SLOTS:
  void setPosition(const Vec &pos);
  void setOrientation(const Quaternion &q);
  void setOrientation(qreal theta, qreal phi);
  void setUpVector(const Vec &up, bool noMove = true);
  void setViewDirection(const Vec &direction);
  //@}

  /*! @name Positioning tools */
  //@{
public Q_SLOTS:
  void lookAt(const Vec &target);
  void showEntireScene();
  void fitSphere(const Vec &center, qreal radius);
  void fitBoundingBox(const Vec &min, const Vec &max);
  void fitScreenRegion(const QRect &rectangle);
  void centerScene();
  void interpolateToZoomOnPixel(const QPoint &pixel);
  void interpolateToFitScene();
  void interpolateTo(const Frame &fr, qreal duration);
  //@}

  /*! @name Frustum */
  //@{
public:
  /*! Returns the Camera::Type of the Camera.

  Set by setType(). Mainly used by loadProjectionMatrix().

  A Camera::PERSPECTIVE Camera uses a classical projection mainly defined by its
  fieldOfView().

  With a Camera::ORTHOGRAPHIC type(), the fieldOfView() is meaningless and the
  width and height of the Camera frustum are inferred from the distance to the
  pivotPoint() using getOrthoWidthHeight().

  Both types use zNear() and zFar() (to define their clipping planes) and
  aspectRatio() (for frustum shape). */
  Type type() const { return type_; }

  /*! Returns the vertical field of view of the Camera (in radians).

  Value is set using setFieldOfView(). Default value is pi/4 radians. This value
  is meaningless if the Camera type() is Camera::ORTHOGRAPHIC.

  The field of view corresponds the one used in \c gluPerspective (see manual).
  It sets the Y (vertical) aperture of the Camera. The X (horizontal) angle is
  inferred from the window aspect ratio (see aspectRatio() and
  horizontalFieldOfView()).

  Use setFOVToFitScene() to adapt the fieldOfView() to a given scene. */
  qreal fieldOfView() const { return fieldOfView_; }

  /*! Returns the horizontal field of view of the Camera (in radians).

  Value is set using setHorizontalFieldOfView() or setFieldOfView(). These
  values are always linked by: \code horizontalFieldOfView() = 2.0 * atan (
  tan(fieldOfView()/2.0) * aspectRatio() ). \endcode */
  qreal horizontalFieldOfView() const {
    return 2.0 * atan(tan(fieldOfView() / 2.0) * aspectRatio());
  }

  /*! Returns the Camera aspect ratio defined by screenWidth() / screenHeight().

  When the Camera is attached to a QGLViewer, these values and hence the
  aspectRatio() are automatically fitted to the viewer's window aspect ratio
  using setScreenWidthAndHeight(). */
  qreal aspectRatio() const {
    return screenWidth_ / static_cast<qreal>(screenHeight_);
  }
  /*! Returns the width (in pixels) of the Camera screen.

  Set using setScreenWidthAndHeight(). This value is automatically fitted to the
  QGLViewer's window dimensions when the Camera is attached to a QGLViewer. See
  also QOpenGLWidget::width() */
  int screenWidth() const { return screenWidth_; }
  /*! Returns the height (in pixels) of the Camera screen.

  Set using setScreenWidthAndHeight(). This value is automatically fitted to the
  QGLViewer's window dimensions when the Camera is attached to a QGLViewer. See
  also QOpenGLWidget::height() */
  int screenHeight() const { return screenHeight_; }

  /*! Returns the pixel ratio (physical to device independent pixel ratio) of the 
  Camera screen.

  Set using setDevicePixelRatio(). Defaults to 1.0. 

  This value is automatically fitted to the QGLViewer's screen pixel ratio when the
  Camera is attached to a QGLViewer. See also QWindow::devicePixelRatio() */
  qreal devicePixelRatio() const { return devicePixelRatio_; }

  void getViewport(GLint viewport[4]) const;
  qreal pixelGLRatio(const Vec &position) const;
  float pixelGLRatio(float x, float y, float z) const;

  /*! Returns the coefficient which is used to set zNear() when the Camera is
  inside the sphere defined by sceneCenter() and zClippingCoefficient() *
  sceneRadius().

  In that case, the zNear() value is set to zNearCoefficient() *
  zClippingCoefficient() * sceneRadius(). See the zNear() documentation for
  details.

  Default value is 0.005, which is appropriate for most applications. In case
  you need a high dynamic ZBuffer precision, you can increase this value (~0.1).
  A lower value will prevent clipping of very close objects at the expense of a
  worst Z precision.

  Only meaningful when Camera type is Camera::PERSPECTIVE. */
  qreal zNearCoefficient() const { return zNearCoef_; }
  /*! Returns the coefficient used to position the near and far clipping planes.

  The near (resp. far) clipping plane is positioned at a distance equal to
  zClippingCoefficient() * sceneRadius() in front of (resp. behind) the
  sceneCenter(). This garantees an optimal use of the z-buffer range and
  minimizes aliasing. See the zNear() and zFar() documentations.

  Default value is square root of 3.0 (so that a cube of size sceneRadius() is
  not clipped).

  However, since the sceneRadius() is used for other purposes (see
  showEntireScene(), flySpeed(),
  ...) and you may want to change this value to define more precisely the
  location of the clipping planes. See also zNearCoefficient().

  For a total control on clipping planes' positions, an other option is to
  overload the zNear() and zFar() methods. See the <a
  href="../examples/standardCamera.html">standardCamera example</a>.

  \attention When QGLViewer::cameraPathAreEdited(), this value is set to 5.0 so
  that the Camera paths are not clipped. The previous zClippingCoefficient()
  value is restored back when you leave this mode. */
  qreal zClippingCoefficient() const { return zClippingCoef_; }

  virtual qreal zNear() const;
  virtual qreal zFar() const;
  virtual void getOrthoWidthHeight(GLdouble &halfWidth,
                                   GLdouble &halfHeight) const;
  void getFrustumPlanesCoefficients(GLdouble coef[6][4]) const;

public Q_SLOTS:
  void setType(Type type);

  void setFieldOfView(qreal fov);

  /*! Sets the horizontalFieldOfView() of the Camera (in radians).

  horizontalFieldOfView() and fieldOfView() are linked by the aspectRatio().
  This method actually calls setFieldOfView(( 2.0 * atan (tan(hfov / 2.0) /
  aspectRatio()) )) so that a call to horizontalFieldOfView() returns the
  expected value. */
  void setHorizontalFieldOfView(qreal hfov) {
    setFieldOfView(2.0 * atan(tan(hfov / 2.0) / aspectRatio()));
  }

  void setFOVToFitScene();

  /*! Defines the Camera aspectRatio().

  This value is actually inferred from the screenWidth() / screenHeight() ratio.
  You should use setScreenWidthAndHeight() instead.

  This method might however be convenient when the Camera is not associated with
  a QGLViewer. It actually sets the screenHeight() to 100 and the screenWidth()
  accordingly. See also setFOVToFitScene().

  \note If you absolutely need an aspectRatio() that does not correspond to your
  viewer's window dimensions, overload loadProjectionMatrix() or multiply the
  created GL_PROJECTION matrix by a scaled diagonal matrix in your
  QGLViewer::draw() method. */
  void setAspectRatio(qreal aspect) {
    setScreenWidthAndHeight(int(100.0 * aspect), 100);
  }

  void setScreenWidthAndHeight(int width, int height);
  void setDevicePixelRatio(qreal ratio);

  /*! Sets the zNearCoefficient() value. */
  void setZNearCoefficient(qreal coef) {
    zNearCoef_ = coef;
    projectionMatrixIsUpToDate_ = false;
  }
  /*! Sets the zClippingCoefficient() value. */
  void setZClippingCoefficient(qreal coef) {
    zClippingCoef_ = coef;
    projectionMatrixIsUpToDate_ = false;
  }
  //@}

  /*! @name Scene radius and center */
  //@{
public:
  /*! Returns the radius of the scene observed by the Camera.

  You need to provide such an approximation of the scene dimensions so that the
  Camera can adapt its zNear() and zFar() values. See the sceneCenter()
  documentation.

  See also setSceneBoundingBox().

  Note that QGLViewer::sceneRadius() (resp. QGLViewer::setSceneRadius()) simply
  call this method (resp. setSceneRadius()) on its associated
  QGLViewer::camera(). */
  qreal sceneRadius() const { return sceneRadius_; }

  /*! Returns the position of the scene center, defined in the world coordinate
  system.

  The scene observed by the Camera should be roughly centered on this position,
  and included in a sceneRadius() sphere. This approximate description of the
  scene permits a zNear() and zFar() clipping planes definition, and allows
  convenient positioning methods such as showEntireScene().

  Default value is (0,0,0) (world origin). Use setSceneCenter() to change it.
  See also setSceneBoundingBox().

  Note that QGLViewer::sceneCenter() (resp. QGLViewer::setSceneCenter()) simply
  calls this method (resp. setSceneCenter()) on its associated
  QGLViewer::camera(). */
  Vec sceneCenter() const { return sceneCenter_; }
  qreal distanceToSceneCenter() const;

public Q_SLOTS:
  void setSceneRadius(qreal radius);
  void setSceneCenter(const Vec &center);
  bool setSceneCenterFromPixel(const QPoint &pixel);
  void setSceneBoundingBox(const Vec &min, const Vec &max);
  //@}

  /*! @name Pivot Point */
  //@{
public Q_SLOTS:
  void setPivotPoint(const Vec &point);
  bool setPivotPointFromPixel(const QPoint &pixel);

public:
  Vec pivotPoint() const;

#ifndef DOXYGEN
public Q_SLOTS:
  void setRevolveAroundPoint(const Vec &point);
  bool setRevolveAroundPointFromPixel(const QPoint &pixel);

public:
  Vec revolveAroundPoint() const;
#endif
  //@}

  /*! @name Associated frame */
  //@{
public:
  /*! Returns the ManipulatedCameraFrame attached to the Camera.

  This ManipulatedCameraFrame defines its position() and orientation() and can
  translate mouse events into Camera displacement. Set using setFrame(). */
  ManipulatedCameraFrame *frame() const { return frame_; }
public Q_SLOTS:
  void setFrame(ManipulatedCameraFrame *const mcf);
  //@}

  /*! @name KeyFramed paths */
  //@{
public:
  KeyFrameInterpolator *keyFrameInterpolator(unsigned int i) const;

public Q_SLOTS:
  void setKeyFrameInterpolator(unsigned int i, KeyFrameInterpolator *const kfi);

  virtual void addKeyFrameToPath(unsigned int i);
  virtual void playPath(unsigned int i);
  virtual void deletePath(unsigned int i);
  virtual void resetPath(unsigned int i);
  virtual void drawAllPaths();
  //@}

  /*! @name OpenGL matrices */
  //@{
public:
  virtual void loadProjectionMatrix(bool reset = true) const;
  virtual void loadModelViewMatrix(bool reset = true) const;
  void computeProjectionMatrix() const;
  void computeModelViewMatrix() const;

  virtual void loadProjectionMatrixStereo(bool leftBuffer = true) const;
  virtual void loadModelViewMatrixStereo(bool leftBuffer = true) const;

  void getProjectionMatrix(GLfloat m[16]) const;
  void getProjectionMatrix(GLdouble m[16]) const;

  void getModelViewMatrix(GLfloat m[16]) const;
  void getModelViewMatrix(GLdouble m[16]) const;

  void getModelViewProjectionMatrix(GLfloat m[16]) const;
  void getModelViewProjectionMatrix(GLdouble m[16]) const;
//@}

/*! @name Drawing */
//@{
#ifndef DOXYGEN
  static void drawCamera(qreal scale = 1.0, qreal aspectRatio = 1.33,
                         qreal fieldOfView = qreal(M_PI) / 4.0);
#endif
  virtual void draw(bool drawFarPlane = true, qreal scale = 1.0) const;
  //@}

  /*! @name World to Camera coordinate systems conversions */
  //@{
public:
  Vec cameraCoordinatesOf(const Vec &src) const;
  Vec worldCoordinatesOf(const Vec &src) const;
  void getCameraCoordinatesOf(const qreal src[3], qreal res[3]) const;
  void getWorldCoordinatesOf(const qreal src[3], qreal res[3]) const;
  //@}

  /*! @name 2D screen to 3D world coordinate systems conversions */
  //@{
public:
  Vec projectedCoordinatesOf(const Vec &src, const Frame *frame = nullptr) const;
  Vec unprojectedCoordinatesOf(const Vec &src, const Frame *frame = nullptr) const;
  void getProjectedCoordinatesOf(const qreal src[3], qreal res[3],
                                 const Frame *frame = nullptr) const;
  void getUnprojectedCoordinatesOf(const qreal src[3], qreal res[3],
                                   const Frame *frame = nullptr) const;
  void convertClickToLine(const QPoint &pixel, Vec &orig, Vec &dir) const;
  Vec pointUnderPixel(const QPoint &pixel, bool &found) const;
  //@}

  /*! @name Fly speed */
  //@{
public:
  qreal flySpeed() const;
public Q_SLOTS:
  void setFlySpeed(qreal speed);
  //@}

  /*! @name Stereo parameters */
  //@{
public:
  /*! Returns the user's inter-ocular distance (in meters). Default value is
  0.062m, which fits most people.

  loadProjectionMatrixStereo() uses this value to define the Camera offset and
  frustum. See setIODistance(). */
  qreal IODistance() const { return IODistance_; }

  /*! Returns the physical distance between the user's eyes and the screen (in
  meters).

  physicalDistanceToScreen() and focusDistance() represent the same distance.
  The former is expressed in physical real world units, while the latter is
  expressed in OpenGL virtual world units.

  This is a helper function. It simply returns physicalScreenWidth() / 2.0 /
  tan(horizontalFieldOfView() / 2.0); */
  qreal physicalDistanceToScreen() const {
    return physicalScreenWidth() / 2.0 / tan(horizontalFieldOfView() / 2.0);
  }

  /*! Returns the physical screen width, in meters. Default value is 0.5m
  (average monitor width).

  Used for stereo display only (see loadModelViewMatrixStereo() and
  loadProjectionMatrixStereo()). Set using setPhysicalScreenWidth(). */
  qreal physicalScreenWidth() const { return physicalScreenWidth_; }

  /*! Returns the focus distance used by stereo display, expressed in OpenGL
  units.

  This is the distance in the virtual world between the Camera and the plane
  where the horizontal stereo parallax is null (the stereo left and right
  cameras' lines of sigth cross at this distance).

  This distance is the virtual world equivalent of the real-world
  physicalDistanceToScreen().

  \attention This value is modified by QGLViewer::setSceneRadius(),
  setSceneRadius() and setFieldOfView(). When one of these values is modified,
  focusDistance() is set to sceneRadius() / tan(fieldOfView()/2), which provides
  good results. */
  qreal focusDistance() const { return focusDistance_; }
public Q_SLOTS:
  /*! Sets the IODistance(). */
  void setIODistance(qreal distance) { IODistance_ = distance; }

#ifndef DOXYGEN
  /*! This method is deprecated. Use setPhysicalScreenWidth() instead. */
  void setPhysicalDistanceToScreen(qreal distance) {
    Q_UNUSED(distance);
    qWarning("setPhysicalDistanceToScreen is deprecated, use "
             "setPhysicalScreenWidth instead");
  }
#endif

  /*! Sets the physical screen (monitor or projected wall) width (in meters). */
  void setPhysicalScreenWidth(qreal width) { physicalScreenWidth_ = width; }

  /*! Sets the focusDistance(), in OpenGL scene units. */
  void setFocusDistance(qreal distance) { focusDistance_ = distance; }
  //@}

  /*! @name XML representation */
  //@{
public:
  virtual QDomElement domElement(const QString &name,
                                 QDomDocument &document) const;
public Q_SLOTS:
  virtual void initFromDOMElement(const QDomElement &element);
  //@}

private Q_SLOTS:
  void onFrameModified();

private:
  // F r a m e
  ManipulatedCameraFrame *frame_;

  // C a m e r a   p a r a m e t e r s
  int screenWidth_, screenHeight_; // size of the window, in pixels
  qreal fieldOfView_;              // in radians
  Vec sceneCenter_;
  qreal sceneRadius_; // OpenGL units
  qreal zNearCoef_;
  qreal zClippingCoef_;
  qreal orthoCoef_;
  qreal devicePixelRatio_;
  Type type_;                            // PERSPECTIVE or ORTHOGRAPHIC
  mutable GLdouble modelViewMatrix_[16]; // Buffered model view matrix.
  mutable bool modelViewMatrixIsUpToDate_;
  mutable GLdouble projectionMatrix_[16]; // Buffered projection matrix.
  mutable bool projectionMatrixIsUpToDate_;

  // S t e r e o   p a r a m e t e r s
  qreal IODistance_;          // inter-ocular distance, in meters
  qreal focusDistance_;       // in scene units
  qreal physicalScreenWidth_; // in meters

  // P o i n t s   o f   V i e w s   a n d   K e y F r a m e s
  QMap<unsigned int, KeyFrameInterpolator *> kfi_;
  KeyFrameInterpolator *interpolationKfi_;
};

} // namespace qglviewer

#endif // QGLVIEWER_CAMERA_H

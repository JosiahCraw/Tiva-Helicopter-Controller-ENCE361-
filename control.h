
#define HEIGHTSTEP 10
#define YAWSTEP 19
#define MAXHEIGHT 100
#define MINHEIGHT 0
#define MINYAW -180
#define MAXYAW 180
#define ERROR 3
#define ZERO_YAW 0
#define TAKE_OFF_HEIGHT 10

#define DELTA_T 0.01

#define KpMain 1.3
#define KiMain 0.05
#define KdMain 8.9

#define KpTail 1.9
#define KiTail 0.6
#define KdTail 0.03

enum controlStates {LANDING=0, TAKINGOFF, FLYING, LANDED};

void findIndependentYawReference(void);

void setLastRefCrossing(int yawSlotCount);

void setCurrentHeight(int height);

void setCurrentYaw(int yaw);

void setReferenceUp(void);

void setReferenceDown(void);

void setReferenceCW(void);

void setReferenceYaw(int yaw);

void setReferenceHeight(int height);

void setReferenceCCW(void);

char* getMode(void);

void setMode(uint8_t mode);

int getDistanceYaw(void);

int getDistanceHeight(void);

int getReferenceHeight(void);

int getReferenceYaw(void);

void updateYaw(void);

bool canChangeMode(void);

void updateHeight(void);

int getOutputMain(void);

int getOutputTail(void);

void updateControl(void);

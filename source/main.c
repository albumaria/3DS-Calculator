#include <citro2d.h>
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_W 320
#define SCREEN_H 240

#define BUTTON_COLS 4
#define BUTTON_ROWS 4
#define BUTTON_SPACING 8 

#define BUTTON_WIDTH  ((SCREEN_W - (BUTTON_SPACING * (BUTTON_COLS + 1))) / BUTTON_COLS)
#define BUTTON_HEIGHT ((SCREEN_H - (BUTTON_SPACING * (BUTTON_ROWS + 1))) / BUTTON_ROWS)

long firstNumber, secondNumber;
char operation;
char outputBuffer[200];
int outputBufferIndex = -1;
bool topNeedsRedraw = true;

void handleEqualPress() {
	if (operation == '+') {
		firstNumber += secondNumber;
	}
	else if (operation == '-') {
		firstNumber -= secondNumber;
	}
	else if (operation == '*') {
		firstNumber *= secondNumber;
	}
	else if (operation == '/') {
		if (secondNumber != 0) {
			firstNumber /= secondNumber;
		}
	}
	sprintf(outputBuffer, "%ld", firstNumber);
	outputBufferIndex = strlen(outputBuffer) - 1;
	secondNumber = 0;
	operation = '\0';
	topNeedsRedraw = true;
}

void handleUndoPress() {
	if (outputBufferIndex < 0) {
		return;
	}

	if (outputBufferIndex >= 2 && outputBuffer[outputBufferIndex] == ' ' && outputBuffer[outputBufferIndex - 2] == ' ' &&
		(outputBuffer[outputBufferIndex - 1] == '+' || outputBuffer[outputBufferIndex - 1] == '-' || outputBuffer[outputBufferIndex - 1] == '*' || outputBuffer[outputBufferIndex - 1] == '/'))
	{
		outputBufferIndex -= 3;
		outputBuffer[outputBufferIndex + 1] = '\0';
		operation = '\0';
		secondNumber = 0;
	}
	else {
		outputBuffer[outputBufferIndex] = '\0';
		outputBufferIndex--;

		if (operation == '\0') {
			firstNumber = (outputBufferIndex >= 0) ? firstNumber / 10 : 0;
		}
		else {
			secondNumber = (secondNumber > 0) ? secondNumber / 10 : 0;
		}
	}

	topNeedsRedraw = true;
}

void handleAddDigitBuffer(char digit) {
	if (outputBufferIndex < 199) {
		outputBufferIndex++;
		outputBuffer[outputBufferIndex] = digit;
		outputBuffer[outputBufferIndex + 1] = '\0';
		topNeedsRedraw = true;
	}
}

void handleKeepNumber(int digit) {
	if (operation == '\0') {
		firstNumber = firstNumber * 10 + digit;
	}
	else {
		secondNumber = secondNumber * 10 + digit;
	}
}

void handleDigitPress(int digit) {
	char character = '0' + digit;
	handleAddDigitBuffer(character);
	handleKeepNumber(digit);
}

void handleAddOperationBuffer(char op) {
	if (outputBufferIndex < 199) {
		outputBufferIndex += 3;
		outputBuffer[outputBufferIndex - 2] = ' ';
		outputBuffer[outputBufferIndex - 1] = op;
		outputBuffer[outputBufferIndex] = ' ';
		outputBuffer[outputBufferIndex + 1] = '\0';
		topNeedsRedraw = true;
	}
}

void handleKeepOperation(char op) {
	if (operation == '\0') {
		operation = op;
	}
}

void handleOperationPress(char op) {
	if (op == '+' || op == '-' || op == '*' || op == '/') {
		handleAddOperationBuffer(op);
		handleKeepOperation(op);
	}
}

void drawTopScreen() {
	if (!topNeedsRedraw) return;

	consoleClear();
	printf("+------------------------------------------------+");
	printf("\x1b[2;20HCalculator\n");
	printf("\x1b[16;22H%s", outputBuffer);

	printf("\x1b[30;0H+------------------------------------------------+");
	topNeedsRedraw = false;
}

C2D_Image buttonImages[4][4];
C2D_SpriteSheet buttonSheets[4][4];

const char* buttonImageNames[4][4] = {
	{"button7.t3x", "button8.t3x", "button9.t3x", "button+.t3x"},
	{"button4.t3x", "button5.t3x", "button6.t3x", "button-.t3x"},
	{"button1.t3x", "button2.t3x", "button3.t3x", "buttonx.t3x"},
	{"buttonundo.t3x", "button0.t3x", "button=.t3x", "buttond.t3x"}
};

void loadButtonImages() 
{
	for (int row = 0; row < BUTTON_ROWS; ++row) {
		for (int col = 0; col < BUTTON_COLS; ++col) {
			char path[64];
			snprintf(path, sizeof(path), "romfs:/%s", buttonImageNames[row][col]);

			buttonSheets[row][col] = C2D_SpriteSheetLoad(path);
			if (!buttonSheets[row][col]) {
				printf("Failed to load sheet: %s\n", path);
			}
			else {
				buttonImages[row][col] = C2D_SpriteSheetGetImage(buttonSheets[row][col], 0);
			}
		}
	}
}

void drawBottomScreen(C3D_RenderTarget* target) {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(target, C2D_Color32(0, 0, 0, 255));
	C2D_SceneBegin(target);

	C2D_DrawRectSolid(0, 0, 0.0f, SCREEN_W, SCREEN_H, C2D_Color32(242, 242, 242, 255));

	for (int row = 0; row < BUTTON_ROWS; ++row) {
		for (int col = 0; col < BUTTON_COLS; ++col) {
			if (buttonImages[row][col].tex) {
				int x = BUTTON_SPACING + col * (BUTTON_WIDTH + BUTTON_SPACING);
				int y = BUTTON_SPACING + row * (BUTTON_HEIGHT + BUTTON_SPACING);
				C2D_DrawImageAt(buttonImages[row][col], x, y, 0.0f, NULL, 1.0f, 1.0f);
			}
		}
	}

	C3D_FrameEnd(0);
}

void handleButton0() {
	handleDigitPress(0);
}

void handleButton1() {
	handleDigitPress(1);
}

void handleButton2() {
	handleDigitPress(2);
}

void handleButton3() {
	handleDigitPress(3);
}

void handleButton4() {
	handleDigitPress(4);
}

void handleButton5() {
	handleDigitPress(5);
}

void handleButton6() {
	handleDigitPress(6);
}

void handleButton7() {
	handleDigitPress(7);
}

void handleButton8() {
	handleDigitPress(8);
}

void handleButton9() {
	handleDigitPress(9);
}

void handleButtonAdd() {
	handleOperationPress('+');
}

void handleButtonSubtract() {
	handleOperationPress('-');
}

void handleButtonMultiply() {
	handleOperationPress('*');
}

void handleButtonDivide() {
	handleOperationPress('/');
}

void handleButtonUndo() {
	handleUndoPress();
}

void handleButtonEquals() {
	handleEqualPress();
}

void handleOptionButtonPress(int row, int col) {
	switch (row) {
	case 0:
		if (col == 0) handleButton7();
		else if (col == 1) handleButton8();
		else if (col == 2) handleButton9();
		else if (col == 3) handleButtonAdd();
		break;
	case 1:
		if (col == 0) handleButton4();
		else if (col == 1) handleButton5();
		else if (col == 2) handleButton6();
		else if (col == 3) handleButtonSubtract();
		break;
	case 2:
		if (col == 0) handleButton1();
		else if (col == 1) handleButton2();
		else if (col == 2) handleButton3();
		else if (col == 3) handleButtonMultiply();
		break;
	case 3:
		if (col == 0) handleButtonUndo();
		else if (col == 1) handleButton0();
		else if (col == 2) handleButtonEquals();
		else if (col == 3) handleButtonDivide();
		break;
	}
}

bool buttonPressed[BUTTON_ROWS][BUTTON_COLS] = { {false} };
void handleButtonPress(int touchX, int touchY) 
{
	for (int row = 0; row < BUTTON_ROWS; ++row) {
		for (int col = 0; col < BUTTON_COLS; ++col) {
			int xPos = BUTTON_SPACING + col * (BUTTON_WIDTH + BUTTON_SPACING);
			int yPos = BUTTON_SPACING + row * (BUTTON_HEIGHT + BUTTON_SPACING);

			if (touchX >= xPos && touchX <= (xPos + BUTTON_WIDTH) &&
				touchY >= yPos && touchY <= (yPos + BUTTON_HEIGHT)) {

				if (!buttonPressed[row][col]) {
					buttonPressed[row][col] = true;

					handleOptionButtonPress(row, col);
					topNeedsRedraw = true;
				}
			}
		}
	}
}

void handleTouchRelease(int touchX, int touchY)
{
	for (int row = 0; row < BUTTON_ROWS; ++row) {
		for (int col = 0; col < BUTTON_COLS; ++col) {
			int xPos = BUTTON_SPACING + col * (BUTTON_WIDTH + BUTTON_SPACING);
			int yPos = BUTTON_SPACING + row * (BUTTON_HEIGHT + BUTTON_SPACING);

			if (touchX < xPos || touchX >(xPos + BUTTON_WIDTH) ||
				touchY < yPos || touchY >(yPos + BUTTON_HEIGHT)) {
				buttonPressed[row][col] = false;
			}
		}
	}
}

C3D_RenderTarget* initialize() 
{
	gfxInitDefault();

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	consoleInit(GFX_TOP, NULL);
	printf("\x1b[16;0HCalculator");
	outputBuffer[0] = '\0';
	outputBufferIndex = -1;

	if (romfsInit() != 0) {
		printf("Failed to initialize ROMFS!\n");
	}
	if (csndInit() != 0) {
		printf("Failed to initialize CSND!\n");
	}
	loadButtonImages();

	C3D_RenderTarget* bottom_screen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	return bottom_screen;
}

void cleanUp() {
	for (int row = 0; row < BUTTON_ROWS; ++row) {
		for (int col = 0; col < BUTTON_COLS; ++col) {
			C2D_SpriteSheetFree(buttonSheets[row][col]);
		}
	}
	romfsExit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

int main(int argc, char **argv)
{
	C3D_RenderTarget* bottom_screen = initialize();

	while (aptMainLoop())
	{
		hidScanInput();

		touchPosition touch;
		hidTouchRead(&touch);

		if (touch.px != 0 || touch.py != 0) {
			handleButtonPress(touch.px, touch.py);
		}
		else {
			handleTouchRelease(touch.px, touch.py);
		}

		drawTopScreen();
		drawBottomScreen(bottom_screen);

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;

		gspWaitForVBlank();
	}

	cleanUp();
	return 0;
}

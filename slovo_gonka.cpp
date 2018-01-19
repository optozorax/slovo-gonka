#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <codecvt>
#include <sstream>

#include <twg/twg.h>
#include <twg/window/window_ctrl.h>
#include <twg/ctrl/clickable_ctrl.h>
#include <twg/ctrl/brain.h>
#include <twg/ctrl/menu.h>
#include <twg/image/image_drawing.h>

using namespace twg;

//-----------------------------------------------------------------------------
class WordGetter;
class RandomWord;
class WorstWord;
class RandomAllWord;
class ConsistentAllWord;

//-----------------------------------------------------------------------------
enum LocalMessages : int32u;
class WrongRightButton;
class ClickHandler;
class MainHandler;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class WordGetter
{
public:
	/** Здесь должны закрываться файлы и прочая вещь. */
	virtual ~WordGetter() {}

	/** В параметр question помещает текущее слово, которое надо угадать.
		В параметр answers помещает варианты ответа. */
	virtual void getQuestion(std::wstring& question, 
							 std::vector<std::wstring>& answers,
							 int32u answersNum) = 0;
	/** Получает номер ответа, который выбрал пользователь. Возвращет был ли этот ответ правильным, или неправильным. */
	virtual bool answer(int8u answerNo, int8u& correctAnswer) = 0;

	/** Меняет местами язык вопроса и язык ответа. */
	virtual void swapLanguage(void) = 0;

	/** Рисует некоторые данные на экран. */
	virtual void draw(ImageBase* buffer) = 0;

	/** Означает, что текущее слово надо бы хорошо заучить. */
	virtual void needToLearn(void) = 0;
};

//-----------------------------------------------------------------------------
struct CommonStatisticData
{
	CommonStatisticData();
	~CommonStatisticData();

	bool						isLeft;

	std::vector<std::wstring>	left;
	std::vector<std::wstring>	right;
	std::vector<int32>			statLeft;
	std::vector<int32>			statRight;

	int32u 						answerPos;
	int32u						correct;
	int32u						incorrect;
	int32u						number;
	int32u						neutral;
	int32u						minus;
	int32u						plus;

	void countStat(void);
	void swapLanguage(void);

	const std::wstring filename = L"words.txt";
	const std::wstring file1 = L"words_1.txt";
	const std::wstring file2 = L"words_2.txt";
};

//-----------------------------------------------------------------------------
class StatisticGetter : public WordGetter
{
public:
	StatisticGetter(CommonStatisticData& m);
	virtual ~StatisticGetter();
	virtual int32u getQuestionPos(void) = 0;
	virtual void afterSwap(void) = 0;

	//-------------------------------------------------------------------------
	void getQuestion(std::wstring& question, 
					 std::vector<std::wstring>& answers, 
					 int32u answersNum);
	bool answer(int8u answerNo, int8u& correctAnswer);
	void swapLanguage(void);
	void needToLearn(void);
protected:
	CommonStatisticData& 	m;
};

//-----------------------------------------------------------------------------
class RandomWord : public StatisticGetter
{
public:
	RandomWord(CommonStatisticData& m) : StatisticGetter(m) {}

	int32u getQuestionPos(void);
	void afterSwap(void);
	void draw(ImageBase* buffer) {}
};

//-----------------------------------------------------------------------------
class WorstWord : public StatisticGetter
{
public:
	WorstWord(CommonStatisticData& m);
	int32u getQuestionPos(void);
	void afterSwap(void);
	void draw(ImageBase* buffer) {}
private:
	void makePushMas(void);
	void push(int32 no);

	std::vector<int32u>			m_pushMas;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
enum LocalMessages : int32u
{
	CLICK_CLICK = 500,
	BUTTON_CLICK = 501,
	MAIN_INIT = 502,
	WAIT_FOR_CLICK = 503
};

//-----------------------------------------------------------------------------
class WrongRightButton : public ClickableCtrl
{
public:
	enum MyState
	{
		BUTTON_DEFAULT,
		BUTTON_WRONG,
		BUTTON_RIGHT
	};

	WrongRightButton(Point_i a, Point_i b, EventsBase* parent) : 
		ClickableCtrl(parent), 
		m_state(BUTTON_DEFAULT), 
		m_a(a), 
		m_b(b) {}


	void setState(MyState state);
	void setString(std::wstring str);
	void setRect(Point_i a, Point_i b);
private:
	MyState			m_state;
	std::wstring 	m_str;
	Point_i			m_a;
	Point_i			m_b;

	bool isInside(Point_i pos);
	void onClick(void);

	void drawButton(ImageBase* buffer, 
					Color up, 
					Color down, 
					Color border);

	void drawDefault(ImageBase* buffer);
	void drawHover(ImageBase* buffer);
	void drawWhenClick(ImageBase* buffer);
};

//-----------------------------------------------------------------------------
/** Ожидает пока по экрану не нажмут любую кнопку мыши. Все сообщения мыши перехватывает себе. Если включен режим ожидания клика. */
class ClickHandler : public CtrlBase
{
public:
	ClickHandler(EventsBase* parent) : CtrlBase(parent), m_isWait(false) {}
	bool onMouse(Point_i pos, MouseType type);
	bool onMessage(int32u messageNo, void* data);
private:
	bool m_isWait;
};

//-----------------------------------------------------------------------------
/** Класс, отвечающий за чтение и сохранение настроек в программе. */
class Settings
{
public:
	void load(Point_i stdPos, Point_i stdSize, bool stdLanguage, bool stdStat, int32u stdGetter, int32u stdButtonCount);
	void save(Point_i curPos, Point_i curSize, bool curLanguage, bool curSTat, int32u curGetter, int32u curButtonCount);

	Point_i pos;
	Point_i size;
	int32u buttonCount;
	bool isLeftLanguage;
	bool drawStat;
	int32u getter;
};

//-----------------------------------------------------------------------------
/** Основные мозги программы. Получает все сообщения, обрабатывает их. */
class MainHandler : public BrainCtrl
{
public:
	MainHandler(EventsBase* parent);
	~MainHandler();

	void init(void);
	void makeMenu(void);

	//-------------------------------------------------------------------------
	bool onMessageNext(int32u messageNo, void* data);
	bool onResize(Rect rect, SizingType type);
	bool onMove(Point_i newPos);
	void draw(ImageBase* buffer);
private:
	int32u							m_buttonsCount;
	std::vector<WrongRightButton*>	m_buttons;
	std::vector<WordGetter*>		m_getters;
	StaticMenu*						m_menu;
	int32u							m_getter;
	std::wstring					m_question;
	std::vector<std::wstring>		m_answers;
	CommonStatisticData				m_data;
	Settings						m_settings;
	bool							m_isLeft;
	bool							m_drawStat;

	void makeButtons(int32u count);
};

//-----------------------------------------------------------------------------
void writeTextInRectangle(ImageBase* img, std::wstring text, int32u size, Color penClr, Point_i a, Point_i b);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
void writeTextInRectangle(ImageBase* img, std::wstring text, int32u size, Color penClr, Point_i a, Point_i b) {
	ImageDrawing_win img2(img);
	img2.setPen(Pen(1, penClr));
	img2.setTextStyle(TextStyle(size, L"Consolas", TEXT_NONE));


	Point_d textSize;
	textSize = img2.getTextSize(text);
	if ((textSize.x + 5) > (b - a).x) {
		text.insert(text.size() / 2, L"-\n");
		textSize = img2.getTextSize(text);
	}

	Point_d pos((a + b)/2 - textSize/2);
	img2.drawText(pos, text);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CommonStatisticData::CommonStatisticData() : 
	answerPos(0),
	correct(0),
	incorrect(0),
	neutral(0),
	plus(0),
	minus(0),
	isLeft(true) {

	// Read words file
	std::wifstream wfin;
	wfin.open(filename, std::ios_base::in);
	wchar_t buffer[500] = {};

	if (wfin) {
		while (!wfin.eof()) {
			wfin.getline(buffer, 500);
			std::wstring word(buffer);
			
			size_t pos = word.find('\t');
			std::wstring rleft = word.substr(0, pos);
			std::wstring rright = word.substr(pos + 1, word.size() - pos - 1);

			left.push_back(rleft);
			right.push_back(rright);
		}

		wfin.close();
	} else
		messageBox(L"Words file not exist!!!", L"Words file not exist!!!", MESSAGE_OK);

	// See for too low words
	if (left.size() < 15) 
		messageBox(L"Too few words", L"In file " + filename + L" you have less than 15 words. Program will only work when there are 15 words or more.", MESSAGE_OK);

	// Read first statistic file
	wfin.open(file1, std::ios_base::in);
	int32u count = 0;
	int32 stat;

	if (wfin) {
		while (!wfin.eof()) {
			wfin >> stat;
			statLeft.push_back(stat);
		}

		wfin.close();
	}

	// Read second statistic file
	wfin.open(file2, std::ios_base::in);
	count = 0;

	if (wfin) {
		while (!wfin.eof()) {
			wfin >> stat;
			statRight.push_back(stat);
		}

		wfin.close();
	}

	// Align the size of statistic arrays
	while (statLeft.size() < left.size())
		statLeft.push_back(0);

	while (statRight.size() < right.size())
		statRight.push_back(0);

	if (statLeft.size() > left.size()) {
		statLeft.erase(statLeft.begin() + left.size(), statLeft.end());
		statRight.erase(statRight.begin() + right.size(), statRight.end());
	}

	countStat();
}

//-----------------------------------------------------------------------------
CommonStatisticData::~CommonStatisticData() {
	if (!isLeft)
		swapLanguage();

	// Save first statistic file
	std::wofstream fout;
	fout.open(file1, std::ios_base::out);

	for (int i = 0; i < statLeft.size(); ++i) {
		fout << statLeft[i] << L" ";
	}

	fout.close();

	// Save second statistic file
	fout.open(file2, std::ios_base::out);

	for (int i = 0; i < statRight.size(); ++i) {
		fout << statRight[i] << L" ";
	}

	fout.close();
}

//-----------------------------------------------------------------------------
void CommonStatisticData::countStat() {
	// Count types of words
	neutral = 0;
	minus = 0;
	plus = 0;
	for (int i = 0; i < statLeft.size(); ++i) {
		if (statLeft[i] == 0)
			neutral++;
		else
			if (statLeft[i] < 0)
				minus++;
			else
				if (statLeft[i] > 0)
					plus++;
	}
}

//-----------------------------------------------------------------------------
void CommonStatisticData::swapLanguage(void) {
	isLeft = !isLeft;
	swap(left, right);
	swap(statLeft, statRight);

	countStat();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
StatisticGetter::StatisticGetter(CommonStatisticData& m) : m(m) {
}

//-----------------------------------------------------------------------------
StatisticGetter::~StatisticGetter() {
}

//-----------------------------------------------------------------------------
void StatisticGetter::getQuestion(std::wstring& question, 
								  std::vector<std::wstring>& answers, 
								  int32u answersNum) {
	std::vector<int32u> answersPos;

	answers.erase(answers.begin(), answers.end());

	m.number = getQuestionPos();
	m.answerPos = std::rand() * answersNum / RAND_MAX;

	question = m.left[m.number];

	// Generate wrong answers without intersections
	for (int i = 0; i < answersNum; ++i) {
		if (i == m.answerPos)
			answersPos.push_back(m.number);
		else {
			newGeneration:
			int32u wrongPos = std::rand() * m.right.size() / RAND_MAX;

			if (wrongPos == m.number)
				goto newGeneration;
			if (m.left[wrongPos] == m.left[m.number])
				goto newGeneration;
			for (int i = 0; i < answersPos.size(); ++i)
				if (wrongPos == answersPos[i])
					goto newGeneration;

			answersPos.push_back(wrongPos);
		}
	}

	for (int i = 0; i < answersPos.size(); ++i)
		answers.push_back(m.right[answersPos[i]]);
}

//-----------------------------------------------------------------------------
bool StatisticGetter::answer(int8u answerNo, int8u& correntAnswer) {
	correntAnswer = m.answerPos;
	bool returned = answerNo == m.answerPos;
	if (returned)
		m.correct++;
	else
		m.incorrect++;

	if (returned)
		if (m.statLeft[m.number] == 0) {
			m.statLeft[m.number] = 1;
			m.neutral--;
			m.plus++;
		} else
			if (m.statLeft[m.number] < 0) {
				m.statLeft[m.number]++;

				if (m.statLeft[m.number] == 0) {
					m.statLeft[m.number]++;
					m.minus--;
					m.plus++;
				}
			} else
				m.statLeft[m.number]++;
	else
		if (m.statLeft[m.number] == 0) {
			m.statLeft[m.number] = -1;
			m.neutral--;
			m.minus++;
		} else
			if (m.statLeft[m.number] > 0) {
				m.statLeft[m.number] = -1;
				m.plus--;
				m.minus++;
			} else
				m.statLeft[m.number]--;

	return returned;
}

//-----------------------------------------------------------------------------
void StatisticGetter::swapLanguage(void) {
	m.swapLanguage();
	
	afterSwap();
}

void StatisticGetter::needToLearn(void) {
	if (m.statLeft[m.number] > -5) {
		if (m.statLeft[m.number] == 0) {
			m.neutral--;
			m.minus++;
		}
		if (m.statLeft[m.number] > 0) {
			m.plus--;
			m.minus++;
		}
		m.statLeft[m.number] = -5;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int32u RandomWord::getQuestionPos(void) {
	return std::rand() * m.left.size() / RAND_MAX;
}

//-----------------------------------------------------------------------------
void RandomWord::afterSwap(void) {
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int32u WorstWord::getQuestionPos(void) {
	if (m_pushMas.size() == 0)
		makePushMas();
	int32u number = m_pushMas.back();
	m_pushMas.pop_back();
	return number;
}

//-----------------------------------------------------------------------------
WorstWord::WorstWord(CommonStatisticData& m) : StatisticGetter(m) {
	makePushMas();
}

//-----------------------------------------------------------------------------
void WorstWord::afterSwap(void) {
	makePushMas();
	m_pushMas.erase(m_pushMas.begin(), m_pushMas.end());
}

//-----------------------------------------------------------------------------
void WorstWord::push(int32 no) {
	for (int i = 0; i < m.statLeft.size(); ++i) {
		if (m.statLeft[i] == no) 
			m_pushMas.push_back(i);
	}
}

//-----------------------------------------------------------------------------
void WorstWord::makePushMas(void) {
	bool addZeros = false;
	int32 min = 10000;
	for (int i = 0; i < m.statLeft.size(); ++i) {
		if (m.statLeft[i] == 0) {
			addZeros = true;
			break;
		} else
		if (m.statLeft[i] < min)
			min = m.statLeft[i];
	}

	if (addZeros)
		push(0);
	else 
		push(min);

	std::random_shuffle(m_pushMas.begin(), m_pushMas.end());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void WrongRightButton::setState(MyState state) {
	m_state = state;
}

//-----------------------------------------------------------------------------
void WrongRightButton::setString(std::wstring str) {
	m_str = str;
}

//-----------------------------------------------------------------------------
void WrongRightButton::setRect(Point_i a, Point_i b) {
	m_a = a;
	m_b = b;
}

//-----------------------------------------------------------------------------
bool WrongRightButton::isInside(Point_i pos) {
	return pos.inRectangle(m_a, m_b);
}

//-----------------------------------------------------------------------------
void WrongRightButton::onClick(void) {
	sendMessageUp(BUTTON_CLICK, new WrongRightButton*(this));
}

//-----------------------------------------------------------------------------
void WrongRightButton::drawButton(ImageBase* buffer, 
								  Color up, 
								  Color down, 
								  Color border) {
	ImageDrawing_win img(buffer);
	Rect rect(m_a.x, m_a.y, m_b.x, m_b.y);
	
	for (int32 j = rect.ay; j < rect.by; ++j) {
		Color clr = getColorBetween(double(j-rect.ay)/rect.y(), 
			up,
			down);
		for (int32 i = rect.ax; i < rect.bx; ++i)
			img.getPixel(Point_i(i, j)) = clr;
	}

	Polygon_d poly;
	poly.array.push_back(Point_d(rect.ax, rect.ay));
	poly.array.push_back(Point_d(rect.bx, rect.ay));
	poly.array.push_back(Point_d(rect.bx, rect.by));
	poly.array.push_back(Point_d(rect.ax, rect.by));

	img.setPen(Pen(0.5, border));
	img.drawPolyline(poly);

	writeTextInRectangle(buffer, m_str, 16, border, Point_i(rect.ax, rect.ay), Point_i(rect.bx, rect.by));
}

//-----------------------------------------------------------------------------
void WrongRightButton::drawDefault(ImageBase* buffer) {
	if (m_state == BUTTON_DEFAULT) {
		drawButton(buffer, White, rgb(0xed, 0xed, 0xed), Gray);
	} else 
	if (m_state == BUTTON_WRONG) {
		drawButton(buffer, 
				   Red, 
				   getColorBetween(double(0xED)/0xFF, Red, Orange), 
				   getColorBetween(0.5, Red, Black));
	} else 
	if (m_state == BUTTON_RIGHT) {
		drawButton(buffer, 
				   Green, 
				   getColorBetween(double(0xED)/0xFF, Green, White), 
				   getColorBetween(0.5, Green, Black));
	};
}

//-----------------------------------------------------------------------------
void WrongRightButton::drawHover(ImageBase* buffer) {
	if (m_state == BUTTON_DEFAULT)
		drawButton(buffer, White, rgb(0xdc, 0xdc, 0xdc), Gray);
	else
		drawDefault(buffer);
}

//-----------------------------------------------------------------------------
void WrongRightButton::drawWhenClick(ImageBase* buffer) {
	m_a.y -= 2;
	m_b.y -= 2;
	if (m_state == BUTTON_DEFAULT)
		drawButton(buffer, rgb(0xed, 0xed, 0xed), White, Gray);
	else
		drawDefault(buffer);
	m_a.y += 2;
	m_b.y += 2;	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool ClickHandler::onMouse(Point_i pos, MouseType type) {
	if (m_isWait) {
		if (type == MOUSE_L_UP) {
			m_isWait = false;
			sendMessageUp(CLICK_CLICK, nullptr);
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
bool ClickHandler::onMessage(int32u messageNo, void* data) {
	if (messageNo == WAIT_FOR_CLICK) {
		m_isWait = true;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void Settings::load(Point_i stdPos, Point_i stdSize, bool stdLanguage, bool stdStat, int32u stdGetter, int32u stdButtonCount) {
	std::wifstream fin;

	fin.open(L"settings.txt");

	if (fin) {
		fin >> pos.x >> pos.y;
		fin >> size.x >> size.y;
		fin >> isLeftLanguage;
		fin >> drawStat;
		fin >> getter;
		fin >> buttonCount;

		fin.close();
	} else {
		pos = stdPos;
		size = stdSize;
		isLeftLanguage = stdLanguage; 
		drawStat = stdStat;
		getter = stdGetter;
		buttonCount = stdButtonCount;
	}
}

//-----------------------------------------------------------------------------
void Settings::save(Point_i curPos, Point_i curSize, bool curLanguage, bool curStat, int32u curGetter, int32u curButtonCount) {
	std::wofstream fout;

	fout.open(L"settings.txt");
	
	fout << curPos.x << L" " << curPos.y << std::endl;
	fout << curSize.x << L" " << curSize.y << std::endl;
	fout << curLanguage << std::endl;
	fout << curStat << std::endl;
	fout << curGetter << std::endl;
	fout << curButtonCount << std::endl;

	fout.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MainHandler::MainHandler(EventsBase* parent) : 
	BrainCtrl(parent),
	m_buttonsCount(4),
	m_getter(0),
	m_isLeft(true),
	m_drawStat(true),
	m_data() {

	m_settings.load(m_wnd->getPos(), m_wnd->getWindowSize(), m_isLeft, m_drawStat, m_getter, m_buttonsCount);
	m_wnd->setPos(m_settings.pos);
	m_wnd->setWindowSize(m_settings.size);
	m_isLeft = m_settings.isLeftLanguage;
	m_drawStat = m_settings.drawStat;
	m_getter = m_settings.getter;
	m_buttonsCount = m_settings.buttonCount;

	if (m_getter > 1) m_getter = 1;
	if (m_getter < 0) m_getter = 0;

	if (m_buttonsCount > 10) m_buttonsCount = 10;
	if (m_buttonsCount < 1) m_buttonsCount = 1;
}

//-----------------------------------------------------------------------------
MainHandler::~MainHandler() {
	// Вызываются деструкторы режимов
	for (int i = 0; i < m_getters.size(); ++i) {
		delete m_getters[i];
	}

	m_settings.save(m_settings.pos, m_settings.size, m_isLeft, m_drawStat, m_getter, m_buttonsCount);
}

//-----------------------------------------------------------------------------
bool MainHandler::onResize(Rect rect, SizingType type) { 
	Point_i size = m_wnd->getClientSize();
	int32u yOffset = 100;
	int32u buttonPadding = 10;
	int32u ySize = (size.y - yOffset) / m_buttons.size() - 10;

	for (int i = 0; i < m_buttons.size(); ++i) {
		m_buttons[i]->setRect(
			Point_i(buttonPadding, yOffset + buttonPadding*i + ySize*i), 
			Point_i(size.x - buttonPadding, yOffset + buttonPadding*i + ySize*(i + 1)));
	}

	m_settings.size = Point_i(rect.x(), rect.y());

	return true; 
}

//-----------------------------------------------------------------------------
bool MainHandler::onMove(Point_i newPos) {
	m_settings.pos = newPos;
	return false;
}

//-----------------------------------------------------------------------------
void MainHandler::draw(ImageBase* buffer) {
	// Рисуется слово, которое надо угадать
	ImageDrawing_win img(buffer);
	Point_i size = m_wnd->getClientSize();
	int32u yOffset = 100;
	int32u buttonPadding = 10;
	int32u dataSize = 0;

	if (m_drawStat) {
		dataSize = 170;

		Rect rect1(buttonPadding, buttonPadding, buttonPadding + dataSize, yOffset - buttonPadding);

		for (int32 j = rect1.ay; j < rect1.by; ++j) {
			Color clr = getColorBetween(double(j-rect1.ay)/rect1.y(), 
				Gray,
				White);
			for (int32 i = rect1.ax; i < rect1.bx; ++i)
				img.getPixel(Point_i(i, j)) = clr;
		}

		Polygon_d poly1;
		poly1.array.push_back(Point_d(rect1.ax, rect1.ay));
		poly1.array.push_back(Point_d(rect1.bx, rect1.ay));
		poly1.array.push_back(Point_d(rect1.bx, rect1.by));
		poly1.array.push_back(Point_d(rect1.ax, rect1.by));

		img.setPen(Pen(0.5, Black));
		img.drawPolyline(poly1);

		// Пишется например сколько слов угадано, сколько нет, выводится само слово
		std::wstringstream sout;
		img.setTextStyle(TextStyle(14, L"Consolas", TEXT_NONE));

		sout << L"Correct answers: " << m_data.correct;
		Point_i pos(Point_i(13, 15));
		img.setPen(Pen(1, getColorBetween(0.2, Green, Black)));
		img.drawText(pos, sout.str());

		pos.y += img.getTextSize(sout.str()).y;
		std::wstringstream sout2;
		sout2 << L"Incorrect answers: " << m_data.incorrect;
		img.setPen(Pen(1, getColorBetween(0.2, Red, Black)));
		img.drawText(pos, sout2.str());

		pos.y += img.getTextSize(sout.str()).y;
		std::wstringstream sout3;
		sout3 << L"Unexplored words: " << m_data.neutral << std::endl
			<< L"Mistakes: " << m_data.minus << std::endl
			<< L"Correct answers: " << m_data.plus << std::endl;
		img.setPen(Pen(1, getGrayHue(0.9)));
		img.drawText(pos, sout3.str());

		dataSize += buttonPadding;
	}

	Rect rect(dataSize + buttonPadding, buttonPadding, size.x - buttonPadding, yOffset - buttonPadding);

	for (int32 j = rect.ay; j < rect.by; ++j) {
		Color clr = getColorBetween(double(j-rect.ay)/rect.y(), 
			Black,
			Gray);
		for (int32 i = rect.ax; i < rect.bx; ++i)
			img.getPixel(Point_i(i, j)) = clr;
	}

	Polygon_d poly;
	poly.array.push_back(Point_d(rect.ax, rect.ay));
	poly.array.push_back(Point_d(rect.bx, rect.ay));
	poly.array.push_back(Point_d(rect.bx, rect.by));
	poly.array.push_back(Point_d(rect.ax, rect.by));

	img.setPen(Pen(0.5, Black));
	img.drawPolyline(poly);

	writeTextInRectangle(buffer, m_question, 24, White, Point_i(rect.ax, rect.ay), Point_i(rect.bx, rect.by));

	// Рисуются всякие косметические вещи
	m_getters[m_getter]->draw(buffer);
}

//-----------------------------------------------------------------------------
void MainHandler::makeButtons(int32u count) {
	for (int i = 0; i < m_buttons.size(); ++i) {
		m_storage->deleteMe(m_buttons[i]);
	}
	m_buttons.erase(m_buttons.begin(), m_buttons.end());

	Point_i size = m_wnd->getClientSize();
	int32u yOffset = 100;
	int32u buttonPadding = 10;
	int32u ySize = (size.y - yOffset) / count - 10;

	for (int i = 0; i < count; ++i) {
		WrongRightButton* button = new WrongRightButton(
			Point_i(buttonPadding, yOffset + buttonPadding*i + ySize*i), 
			Point_i(size.x - buttonPadding, yOffset + buttonPadding*i + ySize*(i + 1)),
			m_storage);
		m_buttons.push_back(button);
		m_storage->array.push_back(button);
	}
}

//-----------------------------------------------------------------------------
void MainHandler::makeMenu(void) {
	std::wstringstream sout;
	sout << L"=100 Swap language | =101 Need to learn | =102 ";
	if (m_drawStat) 
		sout << L"Disable";
	else
		sout << L"Enable";
	sout << L" statistic | Word count: ";
	sout << m_buttonsCount;
	sout << L" > =1 Count++ | =2 Count-- < Regime > =3 Random | =4 Adjusting <";
	m_menu->change(sout.str());
}

//-----------------------------------------------------------------------------
void MainHandler::init(void) {
	// Создает классы генерации слов
	m_getter = 1;
	m_getters.push_back(new RandomWord(m_data));
	m_getters.push_back(new WorstWord(m_data));

	// Создает клик хандлер
	m_storage->array.push_back(new ClickHandler(m_storage));

	// Создает кнопки
	makeButtons(m_buttonsCount);

	// Создает меню
	m_menu = new StaticMenu(L" a ", m_storage);
	makeMenu();
	m_storage->array.push_back(m_menu);

	onMessage(CLICK_CLICK, nullptr);

	m_wnd->worthRedraw();
}

//-----------------------------------------------------------------------------
bool MainHandler::onMessageNext(int32u messageNo, void* data) {
	if (messageNo == CLICK_CLICK) {
		// Получить следующий вопрос
		m_getters[m_getter]->getQuestion(m_question, m_answers, m_buttons.size());

		// Поставить всем кнопкам нормальный цвет
		// Установить всем кнопкам соответсвующие строки.
		for (int32 i = 0; i < m_buttons.size(); i++) {
			m_buttons[i]->setState(WrongRightButton::BUTTON_DEFAULT);
			m_buttons[i]->setString(m_answers[i]);
		}
	} else
	if (messageNo == BUTTON_CLICK) {
		// Проверить правильный ли ответ
		WrongRightButton** pbutton = (WrongRightButton**)data;
		WrongRightButton* button = *pbutton;
		delete pbutton;

		int32u pos = find(m_buttons.begin(), m_buttons.end(), button) - m_buttons.begin();
		int8u correct = 0;

		if (!m_getters[m_getter]->answer(pos, correct))
			button->setState(WrongRightButton::BUTTON_WRONG);

		m_buttons[correct]->setState(WrongRightButton::BUTTON_RIGHT);
		sendMessageUp(WAIT_FOR_CLICK, nullptr);
	} else
	if (messageNo == MENU_CLICK) {
		// Порядок языка
		if (*((int32u*)data) == 100) {
			m_getters[m_getter]->swapLanguage();
			m_isLeft = !m_isLeft;
			onMessage(CLICK_CLICK, nullptr);
		} else

		// Надо заучить слово
		if (*((int32u*)data) == 101) {
			m_getters[m_getter]->needToLearn();
			onMessage(CLICK_CLICK, nullptr);
		} else

		// Скрыть\показать статистику
		if (*((int32u*)data) == 102) {
			m_drawStat = !m_drawStat;
			makeMenu();
		} else

		// Количество спрашиваемых слов
		if (*((int32u*)data) == 1) {
			if (m_buttonsCount < 10) {
				m_buttonsCount++;
				makeButtons(m_buttonsCount);
				onMessage(CLICK_CLICK, nullptr);
				makeMenu();
			}
		} else
		if (*((int32u*)data) == 2) {
			if (m_buttonsCount > 2) {
				m_buttonsCount--;
				makeButtons(m_buttonsCount);
				onMessage(CLICK_CLICK, nullptr);
				makeMenu();
			}
		} else

		// Выбор режима
		if (*((int32u*)data) == 3) {
			m_getter = 0;
		} else
		if (*((int32u*)data) == 4) {
			m_getter = 1;
		} else
		if (*((int32u*)data) == 5) {
			m_getter = 2;
		} else
		if (*((int32u*)data) == 6) {
			m_getter = 3;
		}
	} else 
		return false;

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	std::locale::global(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	srand(100);

	WindowType type(stdIcon,
		Point_i(100, 100),
		Point_i(450, 400),
		Point_i(300, 300), 
		stdMax,
		L"SlovoGonki - learn words",
		WINDOW_STANDART);
	WindowCtrl wnd(type);

	wnd.storage.OMFOC = true;
	MainHandler* handler = new MainHandler(&wnd.storage);
	wnd.storage.array.push_back(handler);

	wnd.waitForClose();
}

/*
	[x] Что происходит когда случайный вариант ответа показывает на текущий  правильный? ответ - говорит что непраивильно. Чтобы не было повторяющихся вариантов ответа
	[x] Чтобы в рандомном режиме так же запоминалось как хорошо отвечаю на некоторые вопросы.
	[x] Чтобы можно было отмечать, какое слово надо заучить (-5)
	[x] Чтобы при правильном ответе отрицательные значения увеличивались на +1
	[x] Чтобы все классы наследовались от статистического анализатора и в соответствии с этим создавали вопросы и т.д.
	[x] Добавить минимальное количество слов, с которыми программа может работать.
	[x] Чтобы в кнопке переносилось на новую строчку
	[x] чтобы не попадались слова с одинаковыым английским написанием
	[ ] Рассмотреть падение программы при смене фокуса
	[ ] Чтобы можно было полистать слова сбоку, почитать, повторить, и показывало какие не изучены, какие правильно отвечены, какие неправильно.
	[x] Чтобы была статистика:
		Сколько всего было ответов, среднее значение знания слов
		Сколько всего было правильных и неправильных
	[x] Чтобы запоминалось сколько кнопок, размер экрана, текущий язык
	[x] Файлы words_1.txt _2 создаются автоматически, если их нет. Если нет файла words.txt, то это пишется в мессадж боксе.
	[x] Создать отдельную программку для того, чтобы наполнять файл words случайными словами из отдельного словаря.
	[x] Чтобы можно было отключить счетчик изученных слов
*/
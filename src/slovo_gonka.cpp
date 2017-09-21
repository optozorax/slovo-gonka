#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <codecvt>
#include <sstream>

#include <twg/twg.h>
#include <twg/window/window_ctrl.h>
#include <twg/ctrl/clickable_ctrl.h>
#include <twg/ctrl/menu.h>
#include <twg/image/image_drawing.h>

using namespace twg;

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
};

//-----------------------------------------------------------------------------
class RandomWord : public WordGetter
{
public:
	RandomWord();
	~RandomWord();

	void getQuestion(std::wstring& question, std::vector<std::wstring>& answers, int32u answersNum);
	bool answer(int8u answerNo, int8u& correctAnswer);
	void swapLanguage(void);
	void draw(ImageBase* buffer);
private:
	std::vector<std::wstring>	m_left;
	std::vector<std::wstring>	m_right;
	int32u 						m_answerPos;
	int32u						m_correct;
	int32u						m_incorrect;
};

//-----------------------------------------------------------------------------
class WorstWord : public WordGetter
{
public:
	WorstWord();
	~WorstWord();

	void getQuestion(std::wstring& question, std::vector<std::wstring>& answers, int32u answersNum);
	bool answer(int8u answerNo, int8u& correctAnswer);
	void swapLanguage(void);
	void draw(ImageBase* buffer);

	void makePushMas(void);
	void push(int32 no);
private:
	std::vector<std::wstring>	m_left;
	std::vector<std::wstring>	m_right;
	int32u 						m_answerPos;
	int32u						m_correct;
	int32u						m_incorrect;
	int32u						m_number;
	int32u						m_neutral;
	int32u						m_minus;
	int32u						m_plus;
	std::vector<int32>			m_statLeft;
	std::vector<int32>			m_statRight;
	std::vector<int32u>			m_pushMas;
	bool						m_isLeft;
};

// //-----------------------------------------------------------------------------
// class RandomAllWord : public WordGetter
// {
// public:
// 	RandomAllWord(std::string filename);
// 	~RandomAllWord();

// 	void getQuestion(std::string& question, std::vector<std::string> answers);
// 	bool answer(int8u answerNo);
// 	void setQuestionLanguage(bool isLeft);
// };

// //-----------------------------------------------------------------------------
// class ConsistentAllWord : public WordGetter
// {
// public:
// 	ConsistentAllWord(std::string filename);
// 	~ConsistentAllWord();

// 	void getQuestion(std::string& question, std::vector<std::string> answers);
// 	bool answer(int8u answerNo);
// 	void setQuestionLanguage(bool isLeft);
// };

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
/** Основные мозги программы. Получает все сообщения, обрабатывает их. */
class MainHandler : public CtrlBase
{
public:
	MainHandler(EventsBase* parent) : 
		CtrlBase(parent),
		m_buttonsCount(4),
		m_getter(0),
		m_isInited(false) {
		WindowCtrl** wnd = (WindowCtrl**)sendMessageUp(WINDOW_GET_POINTER, nullptr);
		m_wnd = *wnd;
		delete wnd;	
	}
	~MainHandler();
	bool onMessage(int32u messageNo, void* data);
	bool onResize(Rect rect, SizingType type);
	void draw(ImageBase* buffer);

	void makeButtons(int32u count);
private:
	int32u							m_buttonsCount;
	std::vector<WrongRightButton*>	m_buttons;
	std::vector<WordGetter*>		m_getters;
	StaticMenu*						m_menu;
	int32u							m_getter;
	std::wstring					m_question;
	std::vector<std::wstring>		m_answers;
	WindowCtrl*						m_wnd;
	bool							m_isInited;
};

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
RandomWord::RandomWord() :  
	m_answerPos(0),
	m_correct(0),
	m_incorrect(0) {
	m_left.reserve(10000);
	m_right.reserve(10000);

	std::wifstream wfin;
	wfin.open("words.txt", std::ios_base::in);
	wchar_t line[500] = {};

	while (!wfin.eof()) {
		wfin.getline(line, 500);
		std::wstring word(line);
		
		size_t pos = word.find('\t');
		std::wstring left = word.substr(0, pos);
		std::wstring right = word.substr(pos + 1, word.size() - pos - 1);

		m_left.push_back(left);
		m_right.push_back(right);
	}

	wfin.close();
}

//-----------------------------------------------------------------------------
RandomWord::~RandomWord() {
	m_answerPos = 0;
}

//-----------------------------------------------------------------------------
void RandomWord::getQuestion(std::wstring& question, std::vector<std::wstring>& answers, int32u answersNum) {
	answers.erase(answers.begin(), answers.end());

	int32u number = std::rand() * m_left.size() / RAND_MAX;
	m_answerPos = std::rand() * answersNum / RAND_MAX;

	question = m_left[number];
	for (int i = 0; i < answersNum; ++i) {
		if (i == m_answerPos)
			answers.push_back(m_right[number]);
		else
			answers.push_back(m_right[std::rand() * m_right.size() / RAND_MAX]);
	}
}

//-----------------------------------------------------------------------------
bool RandomWord::answer(int8u answerNo, int8u& correntAnswer) {
	correntAnswer = m_answerPos;
	bool returned = answerNo == m_answerPos;
	if (returned)
		m_correct++;
	else
		m_incorrect++;
	return returned;
}

//-----------------------------------------------------------------------------
void RandomWord::swapLanguage(void) {
	swap(m_left, m_right);
}

//-----------------------------------------------------------------------------
void RandomWord::draw(ImageBase* buffer) {
	// Пишется например сколько слов угадано, сколько нет, выводится само слово
	ImageDrawing_win img(buffer);

	std::wstringstream sout;
	img.setTextStyle(TextStyle(14, L"Consolas", TEXT_NONE));

	sout << L"Correct answers: " << m_correct;
	Point_i pos(Point_i(13, 15));
	img.setPen(Pen(1, getColorBetween(0.2, Green, Black)));
	img.drawText(pos, sout.str());

	pos.y += img.getTextSize(sout.str()).y;
	std::wstringstream sout2;
	sout2 << L"Incorrect answers: " << m_incorrect;
	img.setPen(Pen(1, getColorBetween(0.2, Red, Black)));
	img.drawText(pos, sout2.str());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
WorstWord::WorstWord() : 
	m_answerPos(0),
	m_correct(0),
	m_incorrect(0) {
	m_isLeft = true;

	m_left.reserve(10000);
	m_right.reserve(10000);

	std::wifstream wfin;
	wfin.open("words.txt", std::ios_base::in);
	wchar_t line[500] = {};

	while (!wfin.eof()) {
		wfin.getline(line, 500);
		std::wstring word(line);
		
		size_t pos = word.find('\t');
		std::wstring left = word.substr(0, pos);
		std::wstring right = word.substr(pos + 1, word.size() - pos - 1);

		m_left.push_back(left);
		m_right.push_back(right);
	}

	wfin.close();

	wfin.open("words_1.txt", std::ios_base::in);
	int32u count = 0;
	int32 stat;

	while (!wfin.eof()) {
		wfin >> stat;
		m_statLeft.push_back(stat);
	}

	wfin.close();

	wfin.open("words_2.txt", std::ios_base::in);
	count = 0;

	while (!wfin.eof()) {
		wfin >> stat;
		m_statRight.push_back(stat);
	}

	wfin.close();

	while (m_statLeft.size() < m_left.size())
		m_statLeft.push_back(0);

	while (m_statRight.size() < m_right.size())
		m_statRight.push_back(0);

	if (m_statLeft.size() > m_left.size()) {
		m_statLeft.erase(m_statLeft.begin() + m_left.size(), m_statLeft.end());
		m_statRight.erase(m_statRight.begin() + m_right.size(), m_statRight.end());
	}

	makePushMas();
}

void WorstWord::push(int32 no) {
	for (int i = 0; i < m_statLeft.size(); ++i) {
		if (m_statLeft[i] == no) 
			m_pushMas.push_back(i);
	}
}

//-----------------------------------------------------------------------------
void WorstWord::makePushMas(void) {
	bool addZeros = false;
	int32 min = 10000;
	for (int i = 0; i < m_statLeft.size(); ++i) {
		if (m_statLeft[i] == 0) {
			addZeros = true;
			break;
		} else
		if (m_statLeft[i] < min) {
			min = m_statLeft[i];
		}
	}

	if (addZeros)
		push(0);
	else 
		push(min);

	m_neutral = 0;
	m_plus = 0;
	m_minus = 0;
	for (int i = 0; i < m_statLeft.size(); ++i) {
		if (m_statLeft[i] == 0)
			m_neutral++;
		else
		if (m_statLeft[i] < 0)
			m_minus++;
		else
		if (m_statLeft[i] > 0)
			m_plus++;
	}
}

//-----------------------------------------------------------------------------
WorstWord::~WorstWord() {
	if (!m_isLeft)
		swapLanguage();

	std::wofstream fout;
	fout.open("words_1.txt", std::ios_base::out);

	for (int i = 0; i < m_statLeft.size(); ++i) {
		fout << m_statLeft[i] << L" ";
	}

	fout.close();

	fout.open("words_2.txt", std::ios_base::out);

	for (int i = 0; i < m_statRight.size(); ++i) {
		fout << m_statRight[i] << L" ";
	}

	fout.close();
}

//-----------------------------------------------------------------------------
void WorstWord::getQuestion(std::wstring& question, std::vector<std::wstring>& answers, int32u answersNum) {
	if (m_pushMas.size() == 0)
		makePushMas();

	answers.erase(answers.begin(), answers.end());

	m_number = m_pushMas.back();
	m_pushMas.pop_back();
	m_answerPos = std::rand() * answersNum / RAND_MAX;

	question = m_left[m_number];
	for (int i = 0; i < answersNum; ++i) {
		if (i == m_answerPos)
			answers.push_back(m_right[m_number]);
		else
			answers.push_back(m_right[std::rand() * m_right.size() / RAND_MAX]);
	}
}

//-----------------------------------------------------------------------------
bool WorstWord::answer(int8u answerNo, int8u& correntAnswer) {
	correntAnswer = m_answerPos;
	bool returned = answerNo == m_answerPos;
	if (returned)
		m_correct++;
	else
		m_incorrect++;

	if (returned)
		if (m_statLeft[m_number] == 0) {
			m_statLeft[m_number] = 1;
			m_neutral--;
			m_plus++;
		} else
			if (m_statLeft[m_number] < 0) {
				m_statLeft[m_number] = 1;
				m_minus--;
				m_plus++;
			} else
				m_statLeft[m_number]++;
	else
		if (m_statLeft[m_number] == 0) {
			m_statLeft[m_number] = -1;
			m_neutral--;
			m_minus++;
		} else
			if (m_statLeft[m_number] > 0) {
				m_statLeft[m_number] = -1;
				m_plus--;
				m_minus++;
			} else
				m_statLeft[m_number]--;

	return returned;
}

//-----------------------------------------------------------------------------
void WorstWord::swapLanguage(void) {
	m_isLeft = !m_isLeft;
	swap(m_left, m_right);
	swap(m_statLeft, m_statRight);
	m_pushMas.erase(m_pushMas.begin(), m_pushMas.end());
	makePushMas();
}

//-----------------------------------------------------------------------------
void WorstWord::draw(ImageBase* buffer) {
	// Пишется например сколько слов угадано, сколько нет, выводится само слово
	ImageDrawing_win img(buffer);

	std::wstringstream sout;
	img.setTextStyle(TextStyle(14, L"Consolas", TEXT_NONE));

	sout << L"Correct answers: " << m_correct;
	Point_i pos(Point_i(13, 15));
	img.setPen(Pen(1, getColorBetween(0.2, Green, Black)));
	img.drawText(pos, sout.str());

	pos.y += img.getTextSize(sout.str()).y;
	std::wstringstream sout2;
	sout2 << L"Incorrect answers: " << m_incorrect;
	img.setPen(Pen(1, getColorBetween(0.2, Red, Black)));
	img.drawText(pos, sout2.str());

	pos.y += img.getTextSize(sout.str()).y;
	std::wstringstream sout3;
	sout3 << L"Unexplored words: " << m_neutral << std::endl
		  << L"Mistakes: " << m_minus << std::endl
		  << L"Correct answers: " << m_plus << std::endl;
	img.setPen(Pen(1, getGrayHue(0.9)));
	img.drawText(pos, sout3.str());
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

	img.setPen(Pen(1, border));
	img.setTextStyle(TextStyle(16, L"Consolas", TEXT_NONE));
	Point_d textSize(img.getTextSize(m_str));
	//Point_d pos((poly.array[0] + poly.array[2])/2 - Point_i(textSize.x, -textSize.y)/2 - Point_i(0, 3));
	Point_d pos((poly.array[0] + poly.array[2])/2 - textSize/2);
	img.drawText(pos, m_str);
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
MainHandler::~MainHandler() {
	// Вызываются деструкторы режимов
	for (int i = 0; i < m_getters.size(); ++i) {
		delete m_getters[i];
	}
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
	return true; 
}

//-----------------------------------------------------------------------------
void MainHandler::draw(ImageBase* buffer) {
	// Рисуется слово, которое надо угадать
	ImageDrawing_win img(buffer);
	Point_i size = m_wnd->getClientSize();
	int32u yOffset = 100;
	int32u buttonPadding = 10;
	int32u dataSize = 170;

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

	Rect rect(dataSize + buttonPadding*2, buttonPadding, size.x - buttonPadding, yOffset - buttonPadding);

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

	img.setPen(Pen(1, White));
	img.setTextStyle(TextStyle(24, L"Consolas", TEXT_NONE));
	Point_d textSize(img.getTextSize(m_question));
	Point_d pos((poly.array[0] + poly.array[2])/2 - textSize/2);
	img.drawText(pos, m_question);

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
bool MainHandler::onMessage(int32u messageNo, void* data) {
	if (!m_isInited && messageNo != MAIN_INIT) 
		return onMessage(MAIN_INIT, nullptr);
	if (messageNo == MAIN_INIT) {
		m_isInited = true;
		// Создает классы генерации слов
		m_getter = 1;
		m_getters.push_back(new RandomWord());
		m_getters.push_back(new WorstWord());

		// Создает клик хандлер
		m_storage->array.push_back(new ClickHandler(m_storage));

		// Создает кнопки
		makeButtons(m_buttonsCount);

		// Создает меню
		m_menu = new StaticMenu(L"=100 Swap language | Word count: 4 > =1 Count++ | =2 Count-- < Regime > =3 Random | =4 Adjusting | =5 ~ All words orderly | =6 ~ All words randomly <", m_storage);
		m_storage->array.push_back(m_menu);

		onMessage(CLICK_CLICK, nullptr);
	} else
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
			onMessage(CLICK_CLICK, nullptr);
		} else

		// Количество спрашиваемых слов
		if (*((int32u*)data) == 1) {
			if (m_buttonsCount < 10) {
				m_buttonsCount++;
				makeButtons(m_buttonsCount);
				onMessage(CLICK_CLICK, nullptr);

				std::wstringstream sout;
				sout << L"=100 Swap language | Word count: " << m_buttonsCount << L" > =1 Count++ | =2 Count-- < Regime > =3 Random | =4 Adjusting | =5 ~ All words orderly | =6 ~ All words randomly <";
				m_menu->change(sout.str());
			}
		} else
		if (*((int32u*)data) == 2) {
			if (m_buttonsCount > 2) {
				m_buttonsCount--;
				makeButtons(m_buttonsCount);
				onMessage(CLICK_CLICK, nullptr);

				std::wstringstream sout;
				sout << L"=100 Swap language | Word count: " << m_buttonsCount << L" > =1 Count++ | =2 Count-- < Regime > =3 Random | =4 Adjusting | =5 ~ All words orderly | =6 ~ All words randomly <";
				m_menu->change(sout.str());
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
	[ ] Что происходит когда случайный вариант ответа показывает на текущий  правильный? ответ - говорит что непраивильно. Чтобы не было повторяющихся вариантов ответа
	[ ] Чтобы в рандомном режиме так же запоминалось как хорошо отвечаю на некоторые вопросы.
	[ ] Чтобы слова переносились на новую строчку
	[ ] Чтобы можно было отмечать, какое слово надо заучить (-5)
	[ ] Чтобы при правильном ответе отрицательные значения увеличивались на +1
	[ ] Чтобы был режим - первые 100 слов, вторые 100 слов и т.д.
	[ ] Чтобы все классы наследовались от статистического анализатора и в соответствии с этим создавали вопросы и т.д.
*/
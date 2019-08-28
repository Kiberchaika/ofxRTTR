#include "ofApp.h"

enum class color
{
	red,
	green,
	blue
};

struct point2d
{
	point2d() {}
	point2d(int x_, int y_) : x(x_), y(y_) {}
	int x = 0;
	int y = 0;
};

struct shape
{
	shape(std::string n) : name(n) {}

	void set_visible(bool v) { visible = v; }
	bool get_visible() const { return visible; }

	color color_ = color::blue;
	std::string name = "";
	point2d position;
	std::map<color, point2d> dictionary;

	RTTR_ENABLE()
private:
	bool visible = false;
};

struct circle : shape
{
	circle(std::string n) : shape(n) {}

	double radius = 5.2;
	std::vector<point2d> points;

	int no_serialize = 100;

	RTTR_ENABLE(shape)
};

RTTR_REGISTRATION
{
	rttr::registration::class_<shape>("shape")
		.property("visible", &shape::get_visible, &shape::set_visible)
		.property("color", &shape::color_)
		.property("name", &shape::name)
		.property("position", &shape::position)
		.property("dictionary", &shape::dictionary)
	;

	rttr::registration::class_<circle>("circle")
		.property("radius", &circle::radius)
		.property("points", &circle::points)
		.property("no_serialize", &circle::no_serialize)
		(
			metadata("NO_SERIALIZE", true)
		)
		;

	rttr::registration::class_<point2d>("point2d")
		.constructor()(rttr::policy::ctor::as_object)
		.property("x", &point2d::x)
		.property("y", &point2d::y)
		;


	rttr::registration::enumeration<color>("color")
		(
			value("red", color::red),
			value("blue", color::blue),
			value("green", color::green)
		);
}


//--------------------------------------------------------------
void ofApp::setup()
{
	std::string json_string;

	{
		circle c_1("Circle #1");
		shape& my_shape = c_1;

		c_1.set_visible(true);
		c_1.points = std::vector<point2d>(2, point2d(1, 1));
		c_1.points[1].x = 23;
		c_1.points[1].y = 42;

		c_1.position.x = 12;
		c_1.position.y = 66;

		c_1.radius = 5.123;
		c_1.color_ = color::red;

		// additional braces are needed for a VS 2013 bug
		c_1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };

		c_1.no_serialize = 12345;

		json_string = io::to_json(my_shape); // serialize the circle to 'json_string'
	}

	std::cout << "Circle: c_1:\n" << json_string << std::endl;

	circle c_2("Circle #2"); // create a new empty circle

	io::from_json(json_string, c_2); // deserialize it with the content of 'c_1'
	std::cout << "\n############################################\n" << std::endl;

	std::cout << "Circle c_2:\n" << io::to_json(c_2) << std::endl;

}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

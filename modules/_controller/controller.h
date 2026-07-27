

#pragma once

#include <stdint.h> 


class Controller {

	public:

		Controller();

		void setup(const char *, const char *, const char *, const char *);				

		void loop();

	private:


};


extern Controller controller;

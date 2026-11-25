

#pragma once

#include <stdint.h>

// tier headers the sketch reaches through this file
#include "mqtt.h"
#include "messages.h"


class Controller {

	public:

		Controller();

		void setup(const char *, const char *, const char *, const char *);				

		void loop();

	private:


};


extern Controller controller;

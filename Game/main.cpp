#include <iostream>

#include "application/ApplicationParameters.hpp"
#include "application/Application.hpp"


int main() {
    ApplicationParameters params = ApplicationParametersReader::read("parameters.json");
    Application app(params);
    app.start();
}
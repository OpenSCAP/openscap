/*
 * test_driver.c
 *
 *  Created on: Mar 4, 2009
 *      Author: david.niemoller
 */
int oval_parser_main(int argc, char **argv);

int main(int argc, char **argv)
{
	return oval_parser_main(argc, argv) ? 0 : 1;
}


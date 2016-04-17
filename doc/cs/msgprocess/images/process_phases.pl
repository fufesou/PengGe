use Graph::Easy::Parser;

print Graph::Easy::Parser->new()->from_file("process_phases.txt")->as_svg();

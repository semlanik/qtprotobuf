package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"regexp"
	"strings"
)

func main() {
	if len(os.Args) < 2 {
		os.Exit(1)
	}

	file, err := os.Open(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	messageFinder, err := regexp.Compile("^message\\s+([a-zA-Z0-9_]+)")
	if err != nil {
		log.Fatalf("Invalid regexp %s\n", err)
	}
	
    serviceFinder, err := regexp.Compile("^service\\s+([a-zA-Z0-9_]+)")

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		capture := messageFinder.FindStringSubmatch(scanner.Text())
		if len(capture) == 2 {
            fmt.Printf("%s.h;", strings.ToLower(capture[1]))
		}

        
		capture = serviceFinder.FindStringSubmatch(scanner.Text())
		if len(capture) == 2 {
            fmt.Printf("%sclient.h;", strings.ToLower(capture[1]))
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"regexp"
	"strings"
	filepath "path/filepath"
)

func main() {
	if len(os.Args) < 2 {
		os.Exit(1)
	}

	multi := false
	if len(os.Args) > 2{
		if os.Args[2] == "MULTI" {
			multi = true
		}
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
	if err != nil {
		log.Fatalf("Invalid regexp %s\n", err)
	}

	scanner := bufio.NewScanner(file)
	if multi {
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
	} else {
		//Singlefile version
		enumFinder, err := regexp.Compile("^enum\\s+([a-zA-Z0-9_]+)")
		if err != nil {
			log.Fatalf("Invalid regexp %s\n", err)
		}

		basename := strings.TrimSuffix(os.Args[1], filepath.Ext(os.Args[1]))
		messageFound := false
		serviceFound := false
		for scanner.Scan() {
			capture := messageFinder.FindStringSubmatch(scanner.Text())
			if len(capture) == 2 {
				messageFound = true
			}

			capture = enumFinder.FindStringSubmatch(scanner.Text())
			if len(capture) == 2 {
				messageFound = true
			}

			capture = serviceFinder.FindStringSubmatch(scanner.Text())
			if len(capture) == 2 {
				serviceFound = true
			}
		}

		if messageFound {
			fmt.Printf("%s.qpb.h;", basename)
		}

		if serviceFound {
			fmt.Printf("%s_grpc.qpb.h;", basename)
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"path"
	filepath "path/filepath"
	"regexp"
	"strings"
)

func main() {
	if len(os.Args) < 2 {
		os.Exit(1)
	}

	multi := false
	if len(os.Args) > 2 {
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

	packageFinder, err := regexp.Compile("^package\\s+(.+);")
	if err != nil {
		log.Fatalf("Invalid regexp %s\n", err)
	}

	scanner := bufio.NewScanner(file)

	fullpath := ""
	for scanner.Scan() {
		capture := packageFinder.FindStringSubmatch(scanner.Text())
		if len(capture) == 2 {
			packages := strings.Split(capture[1], ".")
			fullpath = strings.Join(packages, "/")
			fullpath += "/"
			break
		}
	}

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
		if fullpath == "" {
			log.Fatalf("Package is not specified correctly in %s file\n", os.Args[1])
		}
		//Singlefile version
		enumFinder, err := regexp.Compile("^enum\\s+([a-zA-Z0-9_]+)")
		if err != nil {
			log.Fatalf("Invalid regexp %s\n", err)
		}
		basename := path.Base(os.Args[1])
		basename = strings.TrimSuffix(basename, filepath.Ext(basename))
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
			fmt.Printf("%s%s.qpb.h;", fullpath, basename)
		}

		if serviceFound {
			fmt.Printf("%s%s_grpc.qpb.h;", fullpath, basename)
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

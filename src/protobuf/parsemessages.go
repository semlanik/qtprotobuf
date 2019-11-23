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
    //Multifile version
// 	for scanner.Scan() {
// 		capture := messageFinder.FindStringSubmatch(scanner.Text())
// 		if len(capture) == 2 {
//             fmt.Printf("%s.h;", strings.ToLower(capture[1]))
// 		}
// 
//         
// 		capture = serviceFinder.FindStringSubmatch(scanner.Text())
// 		if len(capture) == 2 {
//             fmt.Printf("%sclient.h;", strings.ToLower(capture[1]))
// 		}
// 	}
    
    //Signlefile version
	basename := strings.TrimSuffix(os.Args[1], filepath.Ext(os.Args[1]))
	messageFound := false
	serviceFound := false
	for scanner.Scan() {
		capture := messageFinder.FindStringSubmatch(scanner.Text())
		if len(capture) == 2 {
			messageFound = true
		}

		capture = serviceFinder.FindStringSubmatch(scanner.Text())
		if len(capture) == 2 {
			serviceFound = true
		}
	}
	if messageFound {
        fmt.Printf("%s.pb.h;", basename)
    }
    
    if serviceFound {
        fmt.Printf("%s_grpc.pb.h;", basename)
    }

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

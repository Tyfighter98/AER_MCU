package main

import (
	"fmt"
	"time"
)

func main() {
	ticker := time.NewTicker(time.Second * 3)
	for {
		select {
		case t := <-ticker.C:
			fmt.Println("Current time: ", t)
			go say("world")
		}
	}
}

func scheduler() {
	ticker := time.NewTicker(time.Second)
	for {
		select {
		case t := <-ticker.C:
			fmt.Println("Current time: ", t)
		}
	}
}

func say(s string) {
	for i := 0; i < 5; i++ {
		time.Sleep(100 * time.Millisecond)
		fmt.Println(s)
	}
}

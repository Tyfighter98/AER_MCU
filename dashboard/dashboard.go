package main

import (
	"math/rand"
	"net/http"
	"os"
	"strconv"
	"strings"
	"text/template"
	"time"

	"github.com/go-echarts/go-echarts/charts"
	"github.com/go-echarts/go-echarts/opts"
	"github.com/go-echarts/go-echarts/types"
)

const numLines = 20
const lineSize = 10

type Welcome struct {
	Name string
	Time string
}

type LogData struct {
	time    []string
	trigger []opts.LineData
	bspd    []opts.LineData
}

// generate random data for line chart
func generateLineItems() []opts.LineData {
	items := make([]opts.LineData, 0)
	for i := 0; i < 7; i++ {
		items = append(items, opts.LineData{Value: rand.Intn(300)})
	}
	return items
}

func httpserver(w http.ResponseWriter, _ *http.Request) {
	logData := readLog()
	// create a new line instance
	line := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	line.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "trigger"}),
		charts.WithTitleOpts(opts.Title{
			Title:    "Line example in Westeros theme",
			Subtitle: "Line chart rendered by the http server this time",
		}))

	// Put data into instance
	line.SetXAxis(logData.time).
		AddSeries("Category A", logData.trigger).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	line.Render(w)

	line2 := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	line2.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "bspd"}),
		charts.WithTitleOpts(opts.Title{
			Title:    "Line example in Westeros theme",
			Subtitle: "Line chart rendered by the http server this time",
		}))

	// Put data into instance
	line2.SetXAxis(logData.time).
		AddSeries("Category B", logData.bspd).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	line2.Render(w)

	templates := template.Must(template.ParseFiles("templates/index.html"))
	welcome := Welcome{"Anonymous", time.Now().Format(time.Stamp)}

	if err := templates.ExecuteTemplate(w, "index.html", welcome); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

func readLog() LogData {
	logData := LogData{make([]string, numLines), make([]opts.LineData, numLines), make([]opts.LineData, numLines)}
	file, err := os.Open("test.csv")
	if err != nil {
		panic(err)
	}
	defer file.Close()

	buf := make([]byte, (numLines * lineSize))
	stat, err := os.Stat("test.csv")
	start := stat.Size() - (numLines * lineSize)
	_, err = file.ReadAt(buf, start)
	rawData := string(buf)
	lines := strings.Split(rawData, "\r\n")

	// for i := 0; i < len(lines); i++ {
	// 	fmt.Printf("%s\n", lines[i])
	// }

	for i := 0; i < numLines; i++ {
		// fmt.Printf("%d: %s\n", i, lines[(len(lines)-2)-i])
		values := strings.Split(lines[(len(lines)-2)-i], ",")
		logData.time[(numLines-1)-i] = values[0]
		trigger, err := strconv.Atoi(values[1])
		if err != nil {
			panic(err)
		}
		bspd, err := strconv.Atoi(values[2])
		if err != nil {
			panic(err)
		}
		logData.trigger[(numLines-1)-i] = opts.LineData{Value: trigger}
		logData.bspd[(numLines-1)-i] = opts.LineData{Value: bspd}
	}

	return logData
}

func main() {
	readLog()
	// go scheduler()
	http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir("static"))))
	http.HandleFunc("/", httpserver)
	http.ListenAndServe(":8081", nil)
}

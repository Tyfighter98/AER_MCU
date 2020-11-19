package main

import (
	"math/rand"
	"net/http"
	"os"
	"strconv"
	"strings"
	"text/template"

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
	Time        []string
	TriggerPlot []opts.LineData
	Trigger     []int
	BspdPlot    []opts.LineData
	Bspd        []int
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
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "Trigger"}),
		charts.WithTitleOpts(opts.Title{
			Title:    "Line example in Westeros theme",
			Subtitle: "Line chart rendered by the http server this Time",
		}))

	// Put data into instance
	line.SetXAxis(logData.Time).
		AddSeries("Category A", logData.TriggerPlot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	line.Render(w)

	line2 := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	line2.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "Bspd"}),
		charts.WithTitleOpts(opts.Title{
			Title:    "Line example in Westeros theme",
			Subtitle: "Line chart rendered by the http server this Time",
		}))

	// Put data into instance
	line2.SetXAxis(logData.Time).
		AddSeries("Category B", logData.BspdPlot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	line2.Render(w)

	templates := template.Must(template.ParseFiles("templates/index.html"))
	// welcome := Welcome{"Anonymous", Time.Now().Format(Time.Stamp)}

	if err := templates.ExecuteTemplate(w, "index.html", logData); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

func readLog() LogData {
	logData := LogData{make([]string, numLines), make([]opts.LineData, numLines), make([]int, numLines), make([]opts.LineData, numLines), make([]int, numLines)}
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
		logData.Time[(numLines-1)-i] = values[0]
		Trigger, err := strconv.Atoi(values[1])
		if err != nil {
			panic(err)
		}
		Bspd, err := strconv.Atoi(values[2])
		if err != nil {
			panic(err)
		}
		logData.TriggerPlot[(numLines-1)-i] = opts.LineData{Value: Trigger}
		logData.Trigger[(numLines-1)-i] = Trigger
		logData.BspdPlot[(numLines-1)-i] = opts.LineData{Value: Bspd}
		logData.Bspd[(numLines-1)-i] = Bspd
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

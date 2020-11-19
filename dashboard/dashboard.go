package main

import (
	"html/template"
	"math/rand"
	"net/http"
	"os"
	"strconv"
	"strings"

	"github.com/go-echarts/go-echarts/charts"
	"github.com/go-echarts/go-echarts/opts"
	"github.com/go-echarts/go-echarts/types"
)

const numLines = 20
const lineSize = 16

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
	ImdPlot     []opts.LineData
	Imd         []int
	SdcPlot     []opts.LineData
	Sdc         []int
	Mom1Plot    []opts.LineData
	Mom1        []int
	Mom2Plot    []opts.LineData
	Mom2        []int
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
	triggerLine := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	triggerLine.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "Trigger"}),
		charts.WithTitleOpts(opts.Title{
			Title: "5V Trigger Signal",
		}))

	// Put data into instance
	triggerLine.SetXAxis(logData.Time).
		AddSeries("Category A", logData.TriggerPlot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	triggerLine.Render(w)

	imdLine := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	imdLine.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "IMD"}),
		charts.WithTitleOpts(opts.Title{
			Title: "IMD Signal",
		}))

	// Put data into instance
	imdLine.SetXAxis(logData.Time).
		AddSeries("Category A", logData.ImdPlot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	imdLine.Render(w)

	sdcLine := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	sdcLine.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "SDC"}),
		charts.WithTitleOpts(opts.Title{
			Title: "Shut Down Circuit Signal",
		}))

	// Put data into instance
	sdcLine.SetXAxis(logData.Time).
		AddSeries("Category A", logData.SdcPlot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	sdcLine.Render(w)

	bspdLine := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	bspdLine.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "Bspd"}),
		charts.WithTitleOpts(opts.Title{
			Title: "BSPD Signal",
		}))

	// Put data into instance
	bspdLine.SetXAxis(logData.Time).
		AddSeries("Category B", logData.BspdPlot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	bspdLine.Render(w)

	mom1Line := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	mom1Line.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "MOM1"}),
		charts.WithTitleOpts(opts.Title{
			Title: "MOM1 Signal",
		}))

	// Put data into instance
	mom1Line.SetXAxis(logData.Time).
		AddSeries("Category A", logData.Mom1Plot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	mom1Line.Render(w)

	mom2Line := charts.NewLine()
	// set some global options like Title/Legend/ToolTip or anything else
	mom2Line.SetGlobalOptions(
		charts.WithInitializationOpts(opts.Initialization{Theme: types.ThemeWesteros, Width: "900px", Height: "250px", ChartID: "MOM2"}),
		charts.WithTitleOpts(opts.Title{
			Title: "MOM2 Signal",
		}))

	// Put data into instance
	mom2Line.SetXAxis(logData.Time).
		AddSeries("Category A", logData.Mom2Plot).
		SetSeriesOptions(charts.WithLineChartOpts(opts.LineChart{Smooth: true}))
	mom2Line.Render(w)

	templates := template.Must(template.ParseFiles("templates/index.html"))
	// welcome := Welcome{"Anonymous", Time.Now().Format(Time.Stamp)}

	if err := templates.ExecuteTemplate(w, "index.html", logData); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

func readLog() LogData {
	logData := LogData{
		make([]string, numLines),
		make([]opts.LineData, numLines),
		make([]int, numLines),
		make([]opts.LineData, numLines),
		make([]int, numLines),
		make([]opts.LineData, numLines),
		make([]int, numLines),
		make([]opts.LineData, numLines),
		make([]int, numLines),
		make([]opts.LineData, numLines),
		make([]int, numLines),
		make([]opts.LineData, numLines),
		make([]int, numLines),
	}
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
		val, err := strconv.Atoi(values[1])
		if err != nil {
			panic(err)
		}
		logData.TriggerPlot[(numLines-1)-i] = opts.LineData{Value: val}
		logData.Trigger[(numLines-1)-i] = val

		val, err = strconv.Atoi(values[2])
		if err != nil {
			panic(err)
		}
		logData.ImdPlot[(numLines-1)-i] = opts.LineData{Value: val}
		logData.Imd[(numLines-1)-i] = val

		val, err = strconv.Atoi(values[3])
		if err != nil {
			panic(err)
		}
		logData.SdcPlot[(numLines-1)-i] = opts.LineData{Value: val}
		logData.Sdc[(numLines-1)-i] = val

		val, err = strconv.Atoi(values[4])
		if err != nil {
			panic(err)
		}
		logData.BspdPlot[(numLines-1)-i] = opts.LineData{Value: val}
		logData.Bspd[(numLines-1)-i] = val

		val, err = strconv.Atoi(values[5])
		if err != nil {
			panic(err)
		}
		logData.Mom1Plot[(numLines-1)-i] = opts.LineData{Value: val}
		logData.Mom1[(numLines-1)-i] = val

		val, err = strconv.Atoi(values[6])
		if err != nil {
			panic(err)
		}
		logData.Mom2Plot[(numLines-1)-i] = opts.LineData{Value: val}
		logData.Mom2[(numLines-1)-i] = val
	}

	return logData
}

func main() {
	http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir("static"))))
	http.HandleFunc("/", httpserver)
	http.ListenAndServe(":8081", nil)
}

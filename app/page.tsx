"use client";

import { useEffect, useState } from "react";
import mqtt from "mqtt";
import { Line } from "react-chartjs-2";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";

ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

export default function SAMRTENDashboard() {
  const [inletFlow, setInletFlow] = useState(0);
  const [outletFlow, setOutletFlow] = useState(0);
  const [inletVolume, setInletVolume] = useState(0);
  const [outletVolume, setOutletVolume] = useState(0);
  const [leakStatus, setLeakStatus] = useState("OK");

  useEffect(() => {
    const client = mqtt.connect("wss://test.mosquitto.org:8081");

    client.on("connect", () => {
      console.log("Connected to MQTT broker");
      client.subscribe("leakage/inletFlow");
      client.subscribe("leakage/outletFlow");
      client.subscribe("leakage/inletVolume");
      client.subscribe("leakage/outletVolume");
    });

    client.on("message", (topic, message) => {
      const value = parseFloat(message.toString());
      if (topic === "leakage/inletFlow") setInletFlow(value);
      if (topic === "leakage/outletFlow") setOutletFlow(value);
      if (topic === "leakage/inletVolume") setInletVolume(value);
      if (topic === "leakage/outletVolume") setOutletVolume(value);
    });

    return () => {
      client.end()
    };
  }, []);

  useEffect(() => {
    if (inletFlow - outletFlow > 0.5) setLeakStatus("LEAK");
    else setLeakStatus("OK");
  }, [inletFlow, outletFlow]);

  // Fake data for charts
  const fakeUsageData = Array.from({ length: 10 }, () => Math.random() * 10 + 5);
  const fakePredictedLeaks = Array.from({ length: 10 }, () => Math.random() * 2);

  const flowChartData = {
    labels: Array.from({ length: 10 }, (_, i) => `T-${10 - i}s`),
    datasets: [
      {
        label: "Inlet Flow",
        data: fakeUsageData.map((_, i) => inletFlow + Math.random() * 2 - 1),
        borderColor: "rgba(0, 123, 255, 0.8)",
        backgroundColor: "rgba(0, 123, 255, 0.2)",
        tension: 0.3,
      },
      {
        label: "Outlet Flow",
        data: fakeUsageData.map((_, i) => outletFlow + Math.random() * 2 - 1),
        borderColor: "rgba(40, 167, 69, 0.8)",
        backgroundColor: "rgba(40, 167, 69, 0.2)",
        tension: 0.3,
      },
    ],
  };

  const riskChartData = {
    labels: Array.from({ length: 10 }, (_, i) => `Pipe ${i + 1}`),
    datasets: [
      {
        label: "Leak Risk",
        data: fakePredictedLeaks,
        backgroundColor: fakePredictedLeaks.map((v) =>
          v > 1 ? "rgba(220, 53, 69, 0.7)" : "rgba(255, 193, 7, 0.7)"
        ),
      },
    ],
  };

  return (
    <div className="w-screen h-screen bg-white text-gray-800 p-6 font-sans overflow-hidden">
      {/* Header */}
      <header className="flex justify-between items-center mb-6">
        <h1 className="text-3xl font-bold text-blue-600">SMARTEN Water Dashboard</h1>
        <div className="text-lg font-semibold">
          Status:{" "}
          <span className={leakStatus === "LEAK" ? "text-red-600" : "text-green-600"}>
            {leakStatus}
          </span>
        </div>
      </header>

      {/* Grid Layout */}
      <div className="grid grid-cols-4 grid-rows-2 gap-4 h-[calc(100%-80px)]">
        {/* Inlet */}
        <div className="bg-gray-100 p-4 rounded-xl shadow border border-blue-400 flex flex-col justify-center items-center">
          <h2 className="text-xl font-bold text-blue-500 mb-2">Inlet Sensor</h2>
          <p className="text-lg">Flow: <span className="font-mono">{inletFlow.toFixed(2)} L/min</span></p>
          <p className="text-lg">Volume: <span className="font-mono">{inletVolume.toFixed(2)} L</span></p>
        </div>

        {/* Outlet */}
        <div className="bg-gray-100 p-4 rounded-xl shadow border border-green-400 flex flex-col justify-center items-center">
          <h2 className="text-xl font-bold text-green-500 mb-2">Outlet Sensor</h2>
          <p className="text-lg">Flow: <span className="font-mono">{outletFlow.toFixed(2)} L/min</span></p>
          <p className="text-lg">Volume: <span className="font-mono">{outletVolume.toFixed(2)} L</span></p>
        </div>

        {/* Flow Chart */}
        <div className="bg-gray-100 p-4 rounded-xl shadow border border-blue-300 col-span-2 row-span-1">
          <h2 className="text-xl font-bold text-blue-600 mb-2">Real-Time Flow Chart</h2>
          <Line data={flowChartData} />
        </div>

        {/* Predicted Leak Risk */}
        <div className="bg-gray-100 p-4 rounded-xl shadow border border-yellow-400 flex flex-col justify-center items-center">
          <h2 className="text-xl font-bold text-yellow-600 mb-2">Predicted Leak Risk</h2>
          <Line data={riskChartData as any} />
        </div>

        {/* Other Metrics */}
        <div className="bg-gray-100 p-4 rounded-xl shadow border border-pink-400 flex flex-col justify-center">
          <h2 className="text-xl font-bold text-pink-600 mb-2">Other Metrics</h2>
          <ul className="list-disc pl-5 text-sm text-gray-700">
            <li>Avg. Daily Consumption: 420 L</li>
            <li>Estimated Loss: 12 L</li>
            <li>Next Maintenance: 2 days</li>
            <li>Active Sensors: 4</li>
          </ul>
        </div>
      </div>
    </div>
  );
}

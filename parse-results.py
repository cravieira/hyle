#!/usr/bin/python3

DESCRIPTION='''
This script parses implementation results of Vitis HLS projects and compiles
the data into a single .csv file.
'''

import argparse
import csv
import json
from pathlib import Path
import xml.etree.ElementTree as ET

def parse_project(path: str):
    """docstring for parse_project"""
    d = dict()
    d['name'] = Path(path).stem

    # Csynth report
    json_path = f'{path}/solution1/solution1_data.json'
    with open(json_path, 'r') as f:
        csynth = json.load(f)
    d['top'] = csynth['Top']
    d['latency'] = csynth['ClockInfo']['Latency']
    d['is_combinational'] = csynth['ClockInfo']['IsCombinational']

    # Implementation report
    xml_file = f'{path}/solution1/impl/report/verilog/export_impl.xml'
    tree = ET.parse(xml_file)
    root = tree.getroot()

    d['achieved_cp'] = root.find('TimingReport/AchievedClockPeriod').text
    d['target_cp'] = root.find('TimingReport/TargetClockPeriod').text
    d['cp_met'] = root.find('TimingReport/TIMING_MET').text
    d['bram'] = root.find('AreaReport/Resources/BRAM').text
    d['clb'] = root.find('AreaReport/Resources/CLB').text
    d['dsp'] = root.find('AreaReport/Resources/DSP').text
    d['ff'] = root.find('AreaReport/Resources/FF').text
    d['latch'] = root.find('AreaReport/Resources/LATCH').text
    d['lut'] = root.find('AreaReport/Resources/LUT').text

    return d

def main():
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument(
        'csv_file',
        help='CSV file to be created from parsed implementation reports'
    )
    parser.add_argument(
        'vitis_projects',
        nargs='+',
        help='Paths to Vitis projects with designs successfully exported and implemented'
    )
    args = parser.parse_args()

    rpts = list(map(parse_project, args.vitis_projects))

    # Write CSV
    with open(args.csv_file, 'w', newline='') as csvfile:
        fieldnames = ['name', 'top', 'latency', 'is_combinational', 'achieved_cp', 'target_cp', 'cp_met', 'bram', 'clb', 'dsp', 'ff', 'latch', 'lut']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

        writer.writeheader()
        for rpt in rpts:
            writer.writerow(rpt)

if __name__ == '__main__':
    main()

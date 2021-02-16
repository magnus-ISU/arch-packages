#!/usr/bin/python3

# Helper file to create pdfs with just a couple function calls

from fpdf import FPDF
import sys
import math

bellcurve_path = 'conf/bell_curve.jpeg'

def normal_cdf(x):
    return (1.0 + math.erf(x / math.sqrt(2.0))) / 2.0

pdf_w=210.0
pdf_h=297.0
class PDF(FPDF):
    num_rows = 0
    num_titles = 0
    def add_page(self):
        super(PDF, self).add_page()
        self.num_rows = 0
        self.num_titles = 0
    def add_line(self, x, y, z, w):
        self.set_line_width(0.0)
        self.line(x,y,z,w)
    def add_bellcurve(self, x, y, w, h):
        self.set_xy(x, y)
        self.image(name=bellcurve_path,w=w,h=h)
    def add_title(self, title):
        title_size = 20.0
        title_y = self.num_titles * title_size
        self.set_xy(0.0,title_y)
        self.set_font('Arial', 'B', title_size)
        self.set_text_color(0,0,0)
        self.cell(w=210.0,h=40.0,align='C',txt=title)

        self.num_titles += 1
    def add_row(self, score):
        row_y = 45 + 40 * self.num_rows
        self.set_xy(0.0,row_y)

        self.set_font('Arial', 'B', 14)
        self.set_text_color(0,0,0)
        self.cell(w=210.0,h=15.0,align='C',txt=('Z = %02.2f' % (score)))

        self.add_line(pdf_w / 2 - 30, row_y + 30, pdf_w / 2 + 30, row_y + 30)

        curve_x = pdf_w * 2 / 3
        curve_w = 60
        curve_h = 30
        self.add_bellcurve(curve_x, row_y, 60, 30)

        std_devs_represented = 6.5
        zscore_x = curve_x + curve_w / 2 + score * curve_w / std_devs_represented
        self.add_line(zscore_x, row_y, zscore_x, row_y + curve_h - 2)

        self.set_xy(curve_x + 10, row_y + curve_h + 1)
        self.set_font('Arial', 'B', 11)
        self.cell(w=0.0, h=0.0, align='L',txt='Higher than %02.2f%%' % (100 * normal_cdf(score)))

        self.num_rows += 1

def make_test():
    pdf = PDF(orientation='P', unit='mm', format='A4')

    pdf.add_page()
    pdf.add_title("Test Z-Score Demo Page")
    pdf.add_row(0.5)
    pdf.add_row(1.5)
    pdf.add_row(-1)
    pdf.add_row(2)

    pdf.add_page()
    pdf.add_title("page 2 test")
    pdf.add_row(-0.3)
    pdf.add_row(0)

    pdf.output('test.pdf', 'F')

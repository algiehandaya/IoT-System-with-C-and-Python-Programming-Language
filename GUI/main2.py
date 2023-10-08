import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QPushButton, QLineEdit, QHBoxLayout
from PyQt5.QtGui import QPainter, QColor, QBrush, QPen, QFont
from PyQt5.QtCore import Qt, QTimer, QRect,QPoint
import random

class DisplayPage(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Packet Simulation")
        
        width = 1500
        height = 810
        self.setFixedSize(width, height)

        self.rssi_values = []
        self.textboxes = []

        layout = QVBoxLayout()
        top_layout = QVBoxLayout()
        mid_layout = QVBoxLayout()
        button_layout = QVBoxLayout()

        self.textboxes = []

        label = QLabel(f"Gateway - RSSI Node 1 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        top_layout.addWidget(label)
        top_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Gateway - RSSI Node 2 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        top_layout.addWidget(label)
        top_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Gateway - RSSI Node 3 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        top_layout.addWidget(label)
        top_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Node 2 - RSSI Node 1 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        top_layout.addWidget(label)
        top_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Node 2 - RSSI Node 3 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        top_layout.addWidget(label)
        top_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Node 3 - RSSI Node 1 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        mid_layout.addWidget(label)
        mid_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Node 3 - RSSI Node 2 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        mid_layout.addWidget(label)
        mid_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Node 1 - RSSI Node 2 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        mid_layout.addWidget(label)
        mid_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Node 1 - RSSI Node 3 (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        mid_layout.addWidget(label)
        mid_layout.addWidget(textbox)
        self.textboxes.append(textbox)

        label = QLabel(f"Threshold RSSI (dBm)")
        textbox = QLineEdit()
        textbox.setFixedSize(200, 30)
        mid_layout.addWidget(label)
        mid_layout.addWidget(textbox)
        self.textboxes.append(textbox)


        self.button_random = QPushButton("Random")
        self.button_random.setFixedSize(200, 30)
        self.button_random.clicked.connect(self.random_fill)
        button_layout.addWidget(self.button_random)

        self.button_continue = QPushButton("Set Value")
        self.button_continue.setFixedSize(200, 30)
        self.button_continue.clicked.connect(self.update_display_text)
        button_layout.addWidget(self.button_continue)

        self.button = QPushButton("Simulate")
        self.button.setFixedSize(200, 30)
        self.button.clicked.connect(self.start_animation)
        button_layout.addWidget(self.button)

        layout.addLayout(top_layout)
        layout.addLayout(mid_layout)
        layout.addLayout(button_layout)

        layout.setAlignment(Qt.AlignTop | Qt.AlignLeft) 
        self.setLayout(layout)

        self.button.setDisabled(True)

        self.rect1 = QRect(100, 195, 20, 10)
        self.rect1.moveTo(-100,-100)
        self.rect2 = QRect(100, 195, 20, 10)
        self.rect2.moveTo(-100,-100)
        self.rect3 = QRect(100, 195, 20, 10)
        self.rect3.moveTo(-100,-100)

        self.error_label = QLabel(f"")
        self.error_label.setStyleSheet("color: red")  # Mengatur warna tulisan menjadi merah

        self.timer = QTimer(self)
        self.step = 0

        self.penrect1 = QPen(Qt.black, 2)
        self.brushrect1 = QBrush(QColor(0, 255, 0, 100))

        self.penrect2 = QPen(Qt.black, 2)
        self.brushrect2 = QBrush(QColor(0, 255, 0, 100))

        self.penrect3 = QPen(Qt.red, 2)
        self.brushrect3 = QBrush(QColor(255, 0, 0, 100))

        #koordinat gateway dan node - 600 x 400
        self.gateway_x = int(width * 1/4) + 100
        self.gateway_y = int(height/2) 
        self.node1_x = int(width * 3/4) + 100
        self.node1_y = int(height/2) 
        self.node2_x = int(width/2 - 100) + 100
        self.node2_y = int(height * 1/4) 
        self.node3_x = int(width/2 + 100) + 100
        self.node3_y = int(height * 3/4) 

        self.dispText = [False,False,False,False]

    def random_fill(self):
        gateway_node1 = random.randint(-255, -156)
        gateway_node2 = random.randint(-80, -10)
        gateway_node3 = random.randint(-100, gateway_node2)
        
        node2_rssi3 = random.randint(-95, -35)
        node2_rssi1 = random.randint(-110, node2_rssi3)
        
        node3_rssi2 = random.randint(-85, -35)
        node3_rssi1 = random.randint(-90, node3_rssi2)
            
        node1_rssi3 = random.randint(-90, -60)
        node1_rssi2 = random.randint(-110, node1_rssi3)
        
        self.textboxes[0].setText(str(gateway_node1))
        self.textboxes[1].setText(str(gateway_node2))
        self.textboxes[2].setText(str(gateway_node3))
        self.textboxes[3].setText(str(node2_rssi1))
        self.textboxes[4].setText(str(node2_rssi3))
        self.textboxes[5].setText(str(node3_rssi1))
        self.textboxes[6].setText(str(node3_rssi2))
        self.textboxes[7].setText(str(node1_rssi2))
        self.textboxes[8].setText(str(node1_rssi3))
        self.textboxes[9].setText(str('-155'))

    def update_display_text(self, text_list):
        text_list = [textbox.text() for textbox in self.textboxes]
        
        if any(text is None or text == '' or not text.lstrip('-').isdigit() for text in text_list):
            #self.error_label.setText("Terjadi kesalahan pada isi textbox!")
            self.button.setDisabled(True)
            return
        else:
            self.button.setDisabled(False)
        
        self.rssi_values = text_list
        self.update()

    def paintEvent(self, event):
        qp = QPainter(self)
        qp.setRenderHint(QPainter.Antialiasing)

        radius = 10

        self.draw_circle(qp, self.gateway_x, self.gateway_y, radius) #Gateway
        self.draw_circle(qp, self.node1_x, self.node1_y, radius) #Node 1
        self.draw_circle(qp, self.node2_x, self.node2_y, radius) #Node 2
        self.draw_circle(qp, self.node3_x, self.node3_y, radius) #Node 3

        pen = QPen(Qt.black, 2)
        qp.setPen(pen)

        qp.drawLine(self.gateway_x, self.gateway_y, self.node1_x, self.node1_y)
        qp.drawLine(self.gateway_x, self.gateway_y, self.node2_x, self.node2_y)
        qp.drawLine(self.gateway_x, self.gateway_y, self.node3_x, self.node3_y)
        qp.drawLine(self.node2_x, self.node2_y, self.node3_x, self.node3_y)
        qp.drawLine(self.node1_x, self.node1_y, self.node2_x, self.node2_y)
        qp.drawLine(self.node1_x, self.node1_y, self.node3_x, self.node3_y)
        qp.drawLine(self.node1_x, self.node1_y, self.node2_x, self.node2_y)
    
        qp.setPen(self.penrect1)
        qp.setBrush(self.brushrect1)
        qp.drawRect(self.rect1)

        qp.setPen(self.penrect2)
        qp.setBrush(self.brushrect2)
        qp.drawRect(self.rect2)
        
        qp.setPen(self.penrect3)
        qp.setBrush(self.brushrect3)
        qp.drawRect(self.rect3)

        # Menulis Text
        font = QFont("Arial", 10)
        qp.setFont(font)
        qp.setPen(QPen(Qt.black))
        line_spacing = 25

        qp.drawText(self.gateway_x-110, self.gateway_y, "GATEWAY")
        qp.drawText(self.node2_x, self.node2_y-20, "NODE 2")
        qp.drawText(self.node3_x, self.node3_y+30, "NODE 3")
        qp.drawText(self.node1_x+20, self.node1_y, "NODE 1")

        if self.dispText[0] is True: #Display Text Gateway
            lines = [str(f"RSSI NODE 1 : {self.rssi_values[0]}"), str(f"RSSI NODE 2 : {self.rssi_values[1]}"), str(f"RSSI NODE 3 : {self.rssi_values[2]}")]
            y = self.gateway_y - 100
            for line in lines:
                qp.drawText(self.gateway_x-120, y, line)
                y += line_spacing
            qp.drawText(self.gateway_x-120, self.gateway_y+90, str(f"RSSI THRESHOLD : {self.rssi_values[9]}"))
            
        
        if self.dispText[2] is True: #Display Text Node 2
            lines = [str(f"RSSI NODE 1 : {self.rssi_values[3]}"), str(f"RSSI NODE 3 : {self.rssi_values[4]}")]
            y = self.node2_y - 80
            for line in lines:
                qp.drawText(self.node2_x-30, y, line)
                y += line_spacing
        
        if self.dispText[3] is True: #Display Text Node 3
            lines = [str(f"RSSI NODE 1 : {self.rssi_values[5]}"), str(f"RSSI NODE 2 : {self.rssi_values[6]}")]
            y = self.node3_y + 20
            for line in lines:
                qp.drawText(self.node3_x+100, y, line)
                y += line_spacing

        if self.dispText[1] is True: #Display Text Node 1
            lines = [str(f"RSSI NODE 2 : {self.rssi_values[7]}"), str(f"RSSI NODE 3 : {self.rssi_values[8]}")]
            y = self.node1_y - 70
            for line in lines:
                qp.drawText(self.node1_x-30, y, line)
                y += line_spacing


    def start_animation(self):
        self.dispText[0] = self.dispText[1] = self.dispText[2] = self.dispText[3] =False
        self.timer.timeout.connect(self.step1)
        self.rect1.moveTo(self.node2_x,self.node2_y-5) #start awal message 1 ditaruh di gateway_x
        self.rect2.moveTo(self.node2_x,self.node2_y-5) #start awal message 1 ditaruh di gateway_x
        self.button.setDisabled(True)
        self.button_continue.setDisabled(True)
        self.timer.start(25) #delay 
       
    def step1(self): #gambar dari node 2 menuju node 3 dan node 1
        current_pos_1 = self.rect1.topLeft()
        target_pos_1 = QPoint(self.node1_x, self.node1_y-5)
        current_pos_2 = self.rect2.topLeft()
        target_pos_2 = QPoint(self.node3_x, self.node3_y-5)

        if current_pos_1 == target_pos_1: 
            self.timer.timeout.disconnect(self.step1)
            self.timer.timeout.connect(self.step2)
            return
        else:
            dx_1 = 5 
            dy_1 = (target_pos_1.y() - current_pos_1.y()) / abs(target_pos_1.x() - current_pos_1.x()) * dx_1

            new_pos_1 = QPoint(int(current_pos_1.x() + dx_1), int(current_pos_1.y() + dy_1))
            self.rect1.moveTopLeft(new_pos_1)

        if current_pos_2 != target_pos_2: 
            dx_2 = 5 
            dy_2 = (target_pos_2.y() - current_pos_2.y()) / abs(target_pos_2.x() - current_pos_2.x()) * dx_2

            new_pos_2 = QPoint(int(current_pos_2.x() + dx_2), int(current_pos_2.y() + dy_2))
            self.rect2.moveTopLeft(new_pos_2)
        
        self.penrect1 = QPen(Qt.yellow, 2)
        self.brushrect1 = QBrush(QColor(255, 255, 0, 100))
        self.penrect2 = QPen(Qt.yellow, 2)
        self.brushrect2 = QBrush(QColor(255, 255, 0, 100))

        self.update()
    
    def step2(self): #gambar dari node 3 dan 1 menuju node 2
        current_pos_1 = self.rect1.topLeft()
        target_pos_1 = QPoint(self.node2_x, self.node2_y-5)
        current_pos_2 = self.rect2.topLeft()
        target_pos_2 = QPoint(self.node2_x, self.node2_y-5)

        if current_pos_1 == target_pos_1: 
            self.dispText[2] = True
            self.timer.timeout.disconnect(self.step2)
            self.timer.timeout.connect(self.step3)
            self.rect1.moveTo(self.node3_x,self.node3_y-5) #start awal message 1 ditaruh di gateway_x
            self.rect2.moveTo(self.node3_x,self.node3_y-5) #start awal message 1 ditaruh di gateway_x
            self.update()
            return
        else:
            dx_1 = 5 
            dy_1 = (target_pos_1.y() - current_pos_1.y()) / abs(target_pos_1.x() - current_pos_1.x()) * dx_1

            new_pos_1 = QPoint(int(current_pos_1.x() - dx_1), int(current_pos_1.y() + dy_1))
            self.rect1.moveTopLeft(new_pos_1)

        if current_pos_2 != target_pos_2: 
            dx_2 = 5 
            dy_2 = (target_pos_2.y() - current_pos_2.y()) / abs(target_pos_2.x() - current_pos_2.x()) * dx_2

            new_pos_2 = QPoint(int(current_pos_2.x() - dx_2), int(current_pos_2.y() + dy_2))
            self.rect2.moveTopLeft(new_pos_2)
        
        self.penrect1 = QPen(Qt.green, 2)
        self.brushrect1 = QBrush(QColor(0, 255, 0, 100))
        self.penrect2 = QPen(Qt.green, 2)
        self.brushrect2 = QBrush(QColor(0, 255, 0, 100))

        self.update()

    def step3(self): #gambar dari node 3 menuju node 2 dan node 1
        current_pos_1 = self.rect1.topLeft()
        target_pos_1 = QPoint(self.node1_x, self.node1_y-5)
        current_pos_2 = self.rect2.topLeft()
        target_pos_2 = QPoint(self.node2_x, self.node2_y-5)

        if current_pos_1 == target_pos_1: 
            self.dispText[1] = True
            self.timer.timeout.disconnect(self.step3)
            self.timer.timeout.connect(self.step4)
            return
        else:
            dx_1 = 5 
            dy_1 = (target_pos_1.y() - current_pos_1.y()) / abs(target_pos_1.x() - current_pos_1.x()) * dx_1

            new_pos_1 = QPoint(int(current_pos_1.x() + dx_1), int(current_pos_1.y() + dy_1))
            self.rect1.moveTopLeft(new_pos_1)

        if current_pos_2 != target_pos_2: 
            dx_2 = 5 
            dy_2 = (target_pos_2.y() - current_pos_2.y()) / abs(target_pos_2.x() - current_pos_2.x()) * dx_2

            new_pos_2 = QPoint(int(current_pos_2.x() - dx_2), int(current_pos_2.y() + dy_2))
            self.rect2.moveTopLeft(new_pos_2)
        
        self.penrect1 = QPen(Qt.yellow, 2)
        self.brushrect1 = QBrush(QColor(255, 255, 0, 100))
        self.penrect2 = QPen(Qt.yellow, 2)
        self.brushrect2 = QBrush(QColor(255, 255, 0, 100))

        self.update()
    
    def step4(self): #gambar dari node 1 dan 2 menuju node 3
        current_pos_1 = self.rect1.topLeft()
        target_pos_1 = QPoint(self.node3_x, self.node3_y-5)
        current_pos_2 = self.rect2.topLeft()
        target_pos_2 = QPoint(self.node3_x, self.node3_y-5)

        if current_pos_1 == target_pos_1: 
            self.dispText[3] = True
            self.timer.timeout.disconnect(self.step4)
            self.timer.timeout.connect(self.step5)
            self.rect1.moveTo(self.gateway_x,self.gateway_y-5) 
            self.rect2.moveTo(self.gateway_x,self.gateway_y-5)
            self.rect3.moveTo(self.gateway_x,self.gateway_y-5)
            self.update()
            return
        else:
            dx_1 = 5 
            dy_1 = (target_pos_1.y() - current_pos_1.y()) / abs(target_pos_1.x() - current_pos_1.x()) * dx_1

            new_pos_1 = QPoint(int(current_pos_1.x() - dx_1), int(current_pos_1.y() + dy_1))
            self.rect1.moveTopLeft(new_pos_1)

        if current_pos_2 != target_pos_2: 
            dx_2 = 5 
            dy_2 = (target_pos_2.y() - current_pos_2.y()) / abs(target_pos_2.x() - current_pos_2.x()) * dx_2

            new_pos_2 = QPoint(int(current_pos_2.x() + dx_2), int(current_pos_2.y() + dy_2))
            self.rect2.moveTopLeft(new_pos_2)
        
        self.penrect1 = QPen(Qt.green, 2)
        self.brushrect1 = QBrush(QColor(0, 255, 0, 100))
        self.penrect2 = QPen(Qt.green, 2)
        self.brushrect2 = QBrush(QColor(0, 255, 0, 100))

        self.update()
    
    def step5(self): #gambar dari gateway menuju node 2 dan node 3 dan node 1
        current_pos_1 = self.rect1.topLeft()
        target_pos_1 = QPoint(self.node2_x, self.node2_y-5)
        current_pos_2 = self.rect2.topLeft()
        target_pos_2 = QPoint(self.node3_x, self.node3_y-5)
        current_pos_3 = self.rect3.topLeft()
        target_pos_3 = QPoint(self.node1_x, self.node1_y-5)

        if current_pos_3 == target_pos_3: 
            self.timer.timeout.disconnect(self.step5)
            self.timer.timeout.connect(self.step6)
            self.penrect3 = QPen(Qt.red, 2)
            self.brushrect3 = QBrush(QColor(255, 0, 0, 100))
            return
        else:
            dx_3 = 5 
            new_pos_3 = QPoint(int(current_pos_3.x() + dx_3), int(current_pos_3.y()))
            self.rect3.moveTopLeft(new_pos_3)
        
        if current_pos_1 != target_pos_1:
            dx_1 = 5 
            dy_1 = (target_pos_1.y() - current_pos_1.y()) / abs(target_pos_1.x() - current_pos_1.x()) * dx_1
            
            new_pos_1 = QPoint(int(current_pos_1.x() + dx_1), int(current_pos_1.y() + dy_1))
            self.rect1.moveTopLeft(new_pos_1)

        if current_pos_2 != target_pos_2:
            dx_2 = 5 
            dy_2 = (target_pos_2.y() - current_pos_2.y()) / abs(target_pos_2.x() - current_pos_2.x()) * dx_2
        
            new_pos_2 = QPoint(int(current_pos_2.x() + dx_2), int(current_pos_2.y() + dy_2))
            self.rect2.moveTopLeft(new_pos_2)
        
        self.penrect1 = QPen(Qt.yellow, 2)
        self.brushrect1 = QBrush(QColor(255, 255, 0, 100))
        self.penrect2 = QPen(Qt.yellow, 2)
        self.brushrect2 = QBrush(QColor(255, 255, 0, 100))
        self.penrect3 = QPen(Qt.yellow, 2)
        self.brushrect3 = QBrush(QColor(255, 255, 0, 100))

        self.update()
    
    def step6(self): #
        current_pos_1 = self.rect1.topLeft()
        target_pos_1 = QPoint(self.gateway_x, self.gateway_y-5)
        current_pos_2 = self.rect2.topLeft()
        target_pos_2 = QPoint(self.gateway_x, self.gateway_y-5)

        targetReached = False

        if int(self.rssi_values[1]) > int(self.rssi_values[9]) and int(self.rssi_values[2]) > int(self.rssi_values[9]):
            if current_pos_2 == target_pos_2:
                targetReached = True
        elif int(self.rssi_values[1]) > int(self.rssi_values[9]) and int(self.rssi_values[2]) < int(self.rssi_values[9]) and current_pos_1 == target_pos_1:
             targetReached = True
        elif int(self.rssi_values[2]) > int(self.rssi_values[9]) and int(self.rssi_values[1]) < int(self.rssi_values[9]) and current_pos_2 == target_pos_2:
             targetReached = True

        if targetReached == True or self.rssi_values[9] == '' or (int(self.rssi_values[1]) <= int(self.rssi_values[9]) and int(self.rssi_values[2]) <= int(self.rssi_values[9])): 
            self.dispText[0] = True
            self.timer.timeout.disconnect(self.step6)
            self.timer.stop()
            self.rect1.moveTo(-100,-100) 
            self.rect2.moveTo(-100,-100)
            self.rect3.moveTo(-100,-100)
            self.button.setDisabled(False)
            self.button_continue.setDisabled(False)
            self.update()
            return
            
        if int(self.rssi_values[1]) > int(self.rssi_values[9]):
            if current_pos_1 != target_pos_1:
                dx_1 = 5 
                dy_1 = (target_pos_1.y() - current_pos_1.y()) / abs(target_pos_1.x() - current_pos_1.x()) * dx_1
                new_pos_1 = QPoint(int(current_pos_1.x() - dx_1), int(current_pos_1.y() + dy_1))
                self.rect1.moveTopLeft(new_pos_1)
                self.penrect1 = QPen(Qt.green, 2)
                self.brushrect1 = QBrush(QColor(0, 255, 0, 100))
        else:
            self.penrect1 = QPen(Qt.red, 2)
            self.brushrect1 = QBrush(QColor(255, 0, 0, 100))

        if int(self.rssi_values[2]) > int(self.rssi_values[9]):
            if current_pos_2 != target_pos_2:
                dx_2 = 5 
                dy_2 = (target_pos_2.y() - current_pos_2.y()) / abs(target_pos_2.x() - current_pos_2.x()) * dx_2
                new_pos_2 = QPoint(int(current_pos_2.x() - dx_2), int(current_pos_2.y() + dy_2))
                self.rect2.moveTopLeft(new_pos_2)
                self.penrect2 = QPen(Qt.green, 2)
                self.brushrect2 = QBrush(QColor(0, 255, 0, 100))
        else:
            self.penrect2 = QPen(Qt.red, 2)
            self.brushrect2 = QBrush(QColor(255, 0, 0, 100))

        self.update()

    def draw_circle(self, qp, x, y, radius):
        brush = QBrush(QColor(0, 255, 0, 100))
        qp.setBrush(brush)
        qp.drawEllipse(x - radius, y - radius, 2 * radius, 2 * radius)

    def closeEvent(self, event):
        sys.exit(app.exec_())

if __name__ == "__main__":
    app = QApplication(sys.argv)

    display_page = DisplayPage()
    display_page.show()

    sys.exit(app.exec_())

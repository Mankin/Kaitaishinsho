#include "testApp.h"

/* Note on OS X, you must have this in the Run Script Build Phase of your project. 
where the first path ../../../addons/ofxLeapMotion/ is the path to the ofxLeapMotion addon. 

cp -f ../../../addons/ofxLeapMotion/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";

   If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib
*/

ofPoint pt;

deque< pair<float, float> > pts;
// Create Iterater.
deque< pair<float, float> >::iterator iter;

int threshold_of_pts = 10;


pair<float, float> additional_pts_pair;


//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);

	leap.open(); 

	l1.setPosition(200, 300, 50);
	l2.setPosition(-200, -200, 50);

	cam.setOrientation(ofPoint(-20, 0, 0));

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    /*
     * For Reading Image File.
     */
    //画面の基本設定
    ofBackground(0,0,0);
    ofEnableSmoothing();
    //画面の混色の設定を加算合成にする
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //画像データの読込み
    myImage.loadImage("komine.jpg");
    myImage_2.loadImage("komine2.jpg");
}


//--------------------------------------------------------------
void testApp::update(){

	fingersFound.clear();
	
	//here is a simple example of getting the hands and using them to draw trails from the fingertips. 
	//the leap data is delivered in a threaded callback - so it can be easier to work with this copied hand data
	
	//if instead you want to get the data as it comes in then you can inherit ofxLeapMotion and implement the onFrame method. 
	//there you can work with the frame data directly. 



    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms. 
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
    
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
    
        for(int i = 0; i < simpleHands.size(); i++){
        
            //for(int j = 0; j < simpleHands[i].fingers.size(); j++){
            for(int j = 0; j < 2; j++){
                int id = simpleHands[i].fingers[1].id;
                
                ofPolyline & polyline = fingerTrails[id];
                /*
                 * Get the position of the finger.
                 */
                /*
                 * ptをqueueに変えたので値の追加方法を変更
                 */
                //pt.push(simpleHands[i].fingers[1].pos);
                pt = simpleHands[i].fingers[1].pos;
                //std::cout << "ptだよ！";
                //std:cout << pt;
            
                
                //if the distance between the last point and the current point is too big - lets clear the line 
                //this stops us connecting to an old drawing
                if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
                    polyline.clear(); 
                }
                
                //add our point to our trail
                polyline.addVertex(pt);
                
                //store fingers seen this frame for drawing
                fingersFound.push_back(id);
            }
        }
    }


    // Option 2: Work with the leap data / sdk directly - gives you access to more properties than the simple approach  
    // uncomment code below and comment the code above to use this approach. You can also inhereit ofxLeapMotion and get the data directly via the onFrame callback. 
    
//	vector <Hand> hands = leap.getLeapHands(); 
//	if( leap.isFrameNew() && hands.size() ){
//
//		//leap returns data in mm - lets set a mapping to our world space. 
//		//you can get back a mapped point by using ofxLeapMotion::getMappedofPoint with the Leap::Vector that tipPosition returns  
//		leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
//		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
//		leap.setMappingZ(-150, 150, -200, 200);
//				
//		for(int i = 0; i < hands.size(); i++){
//            for(int j = 0; j < hands[i].fingers().count(); j++){
//				ofPoint pt; 
//			
//				const Finger & finger = hands[i].fingers()[j];
//								
//				//here we convert the Leap point to an ofPoint - with mapping of coordinates
//				//if you just want the raw point - use ofxLeapMotion::getofPoint 
//				pt = leap.getMappedofPoint( finger.tipPosition() );
//                								
//				//lets get the correct trail (ofPolyline) out of our map - using the finger id as the key 
//				ofPolyline & polyline = fingerTrails[finger.id()]; 
//				
//				//if the distance between the last point and the current point is too big - lets clear the line 
//				//this stops us connecting to an old drawing
//				if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
//					polyline.clear(); 
//				}
//				
//				//add our point to our trail
//				polyline.addVertex(pt); 
//				
//				//store fingers seen this frame for drawing
//				fingersFound.push_back(finger.id());
//			}
//		}	
//	}
//    

	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new. 
	leap.markFrameAsOld();	
}

//--------------------------------------------------------------
void testApp::draw(){
    
    /*
     * For Reading Image File.
     */
    //色の設定
    ofSetColor(255, 255, 255);
    //読み込んだ画像データを画面に描画

    //画像データのビットマップ情報を配列に格納
    unsigned char * pixels = myImage.getPixels();
    unsigned char * pixels_2 = myImage_2.getPixels();
    
    // Mask
    int width = 960;
    int height = 640;
    unsigned char mask[width * height];
    for (int i=0; i < (width * height) + 1; i++) {
        // すべてのピクセルを黒に初期化する
        mask[i] = 0;
    }
    //画像の幅と高さを所得
    int w = myImage.width;
    int h = myImage.height;
    
    // 指の座標を取得
    /*x.push_back(pt.x);
    y.push_back(pt.y);*/
    
    additional_pts_pair.first = pt.x;
    additional_pts_pair.second = pt.y;
    
    // ptsの要素数を取得
    int size_of_pts = pts.size();
    // 閾値と比較
    if (size_of_pts > threshold_of_pts) {
        std::cout << "実行！";
        std::cout << "threshold_of_pts";
        std::cout << threshold_of_pts;
        std::cout << "size_of_pts";
        std::cout << size_of_pts;
        pts.pop_back();
    }
    // 値追加
    pts.push_front(additional_pts_pair);
    
    /*
     * 座標の周囲を四角形で白に変更
     */
    for( iter = pts.begin(); iter != pts.end(); iter++ ){
        // maskを白に
        int pt_to_be_changed = width*(*iter).second+(*iter).first;
        // 周囲のピクセルも白に
        if (pt_to_be_changed > width && pt_to_be_changed < width*height-1-width) {
            int pixels_near_pt_to_be_changed[] = {
                pt_to_be_changed - width,
                pt_to_be_changed -1,
                pt_to_be_changed,
                pt_to_be_changed + 1,
                pt_to_be_changed + width,
                pt_to_be_changed - width - 1,
                pt_to_be_changed - width + 1,
                pt_to_be_changed + width - 1,
                pt_to_be_changed + width + 1
            };
            int size_of_pixels_near_pt_to_be_changed = sizeof(pixels_near_pt_to_be_changed);
            for (int i=0;i<size_of_pixels_near_pt_to_be_changed;i++) {
                mask[pixels_near_pt_to_be_changed[i]] = 1;
            }
        }
    }
    
    
    
    
    if( leap.isFrameNew() && simpleHands.size() != 0 ){
        
        for( iter = pts.begin(); iter != pts.end(); iter++ ){
            if ((*iter).first != 0) {
                std::cout << "x座標";
                cout << (*iter).first << endl;
                std::cout << "y座標";
                cout << (*iter).second << endl;
            }
        }
        
    
    
    //画像を8ピクセル間隔でスキャン
    for (int i = 0; i < w; i++){
        for (int j = 0; j < h; j++){
            //ピクセルのRGBの値を取得
            //if ((x - i)*(x - i)+(y-j)*(y-j) < 100) {
                /*int valueR = 255;
                int valueG = 255;
                int valueB = 255;
                int valueR = pixels_2[j*3 * w + i*3];
                int valueG = pixels_2[j*3 * w + i*3+1];
                int valueB = pixels_2[j*3 * w + i*3+2];
                ofSetColor(valueR, valueG, valueB);
                ofCircle(i, j, 1);
            }*/
                //std::cout << (*iter).first;
            /*if ((x - i)*(x - i)+(y-j)*(y-j) < 100) {
                int valueR = 255;
                int valueG = 255;
                int valueB = 255;
                int valueR = pixels_2[j*3 * w + i*3];
                int valueG = pixels_2[j*3 * w + i*3+1];
                int valueB = pixels_2[j*3 * w + i*3+2];
                ofSetColor(valueR, valueG, valueB);
                ofCircle(i, j, 1);
            }*/
                 //取得したRGB値をもとに、円を描画
            //取得したピクセルの明るさを、円の半径に対応させている
            /*ofSetColor(255, 0, 0, 63);
            ofCircle(440+i, 20+j, 10*valueR/255.0);
            ofSetColor(0, 255, 0, 63);
            ofCircle(440+i, 20+j, 10*valueG/255.0);
            ofSetColor(0, 0, 255, 63);
            ofCircle(440+i, 20+j, 10*valueB/255.0);*/
            
        }
    }
        
    } // 今実験してるやる終わり
    
    
    // 画像描写
    ofSetColor(255, 255, 255); //おまじない
    myImage.draw(100,20);
    
    //myImage_2.draw(100, 20);
    
    
    
	ofDisableLighting();
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
	
	ofSetColor(200);
	ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);

	cam.begin();	

	ofPushMatrix();
		ofRotate(90, 0, 0, 1);
		ofSetColor(20);
		ofDrawGridPlane(800, 20, false);
	ofPopMatrix();
	
	ofEnableLighting();
	l1.enable();
	l2.enable();
	
	m1.begin(); 
	m1.setShininess(0.6);
	
	for(int i = 0; i < fingersFound.size(); i++){
		ofxStrip strip;
		int id = fingersFound[i];
		
		ofPolyline & polyline = fingerTrails[id];
		strip.generate(polyline.getVertices(), 15, ofPoint(0, 0.5, 0.5) );
		
		ofSetColor(255 - id * 15, 0, id * 25);
		strip.getMesh().draw();
	}
	
    l2.disable();
    
    // 手のオブジェクト入れてるのはこれ！！！
    for(int i = 0; i < simpleHands.size(); i++){
        simpleHands[i].debugDraw();
    }

	m1.end();
	cam.end();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    /*
     * For Reading Image File.
     */
    //「x」キーを押すと、画面をキャプチャーする
    if(key == 'x'){
        //位置とサイズを指定して、画面をキャプチャー
        grabbedImage.grabScreen(430,10,420,642);
        //キャプチャーした画像データを「grabbedImage.png」で保存
        grabbedImage.saveImage("grabbedImage.png");
    }
}
//--------------------------------------------------------------
void makeCircle (deque<float> pt_x, deque<float> pt_y, deque<float> pt_z){

}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void testApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}

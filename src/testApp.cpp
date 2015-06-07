#include "testApp.h"
#include "ofAppGlutWindow.h"

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

int window_width;
int window_height;

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
    // サーバ画像参照
    string image_url_1 = "http://160.16.56.118/hd15/rslt/img/main.jpg";
    string image_url_2 = "http://160.16.56.118/hd15/rslt/img/1.jpg";
    string image_url_3 = "http://160.16.56.118/hd15/rslt/img/2.jpg";
    myImage.loadImage(image_url_1);
    myImage_2.loadImage(image_url_2);
    myImage_3.loadImage(image_url_3);
    
    // ローカル画像参照
    
    /*myImage.loadImage("komine.jpg");
    myImage_2.loadImage("komine2.jpg");
    myImage_3.loadImage("komine3.jpg");*/
    
    window_width = ofGetWidth();  // アプリウィンドウの横ピクセル数を返します．
    window_height = ofGetHeight(); // アプリウィンドウの縦ピクセル数を返します
    
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
                int id = simpleHands[i].fingers[1].id;
                
                //ofPolyline & polyline = fingerTrails[id];
                /*
                 * Get the position of the finger.
                 */
                //pt.push(simpleHands[i].fingers[1].pos);
            pt = simpleHands[i].fingers[1].pos;
            //pt.x -= 20;
            //pt.y -= 100;
            
                //if the distance between the last point and the current point is too big - lets clear the line 
                //this stops us connecting to an old drawing
                /*if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
                    polyline.clear();
                }*/
                
                //add our point to our trail
                //polyline.addVertex(pt);
                
                //store fingers seen this frame for drawing
                //fingersFound.push_back(id);
            //}
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
    unsigned char * pixels_3 = myImage_3.getPixels();

    //画像の幅と高さを所得
    int w = myImage.width;
    int h = myImage.height;
    
    int offset_x = (window_width - w)/2;
    int offset_y = (window_height - h)/2;
    
    // Mask
    int width_of_mask = 960;
    int height_of_mask = 638;
    unsigned char mask[width_of_mask * height_of_mask];
    for (int i=0; i < (width_of_mask * height_of_mask) + 1; i++) {
        // すべてのピクセルを黒に初期化する
        mask[i] = 0;
    }
    
    // 指の座標を取得
    /*x.push_back(pt.x);
    y.push_back(pt.y);*/
    
    
    
    
    if( leap.isFrameNew() && simpleHands.size() != 0 ){ // 手を検知した時
        
        pair<float, float> additional_pts_pair;
        additional_pts_pair.first = pt.x;
        additional_pts_pair.second = pt.y;
        // ptsの要素数を取得
        int size_of_pts = pts.size();
        // 閾値と比較
        if (size_of_pts > threshold_of_pts) {
            pts.pop_back();
        }
        // 値追加
        pts.push_front(additional_pts_pair);
    }
    
    
    bool first_flug = true;
    int current_first;
    int current_second;
    int prev_first;
    int prev_second;
    
    int threshold_of_depth = -40;

    for( iter = pts.begin(); iter != pts.end(); iter++ ){
        
        if (first_flug) {
            first_flug = false;
            
            current_first = (*iter).first;
            current_second = (*iter).second;
            continue;
        } else {
            prev_first = current_first;
            prev_second = current_second;
            
            current_first = (*iter).first;
            current_second = (*iter).second;
        }
        
        int max_f;
        int min_f;
        int max_s;
        int min_s;
        
        
        current_first = round(current_first);
        prev_first = round(prev_first);
        current_second = round(current_second);
        prev_second = round(prev_second);
        
        if (current_first > prev_first) {
            if (current_first < 0) {
                max_f = 0;
            } else {
                max_f = current_first;
            }
            if (prev_first < 0) {
                min_f = 0;
            } else {
                min_f = prev_first;
            }
        } else {
            if (prev_first < 0) {
                max_f = 0;
            } else {
                max_f = prev_first;
            }
            if (current_first < 0) {
                min_f = 0;
            } else {
                min_f = current_first;
            }
        }
        
        if (current_second > prev_second) {
            if (current_second < 0) {
                max_s = 0;
            } else {
                max_s = current_second;
            }
            if (prev_second < 0) {
                min_s = 0;
            } else {
                min_s = prev_second;
            }
        } else {
            if (prev_second < 0) {
                max_s = 0;
            } else {
                max_s = prev_second;
            }
            if (current_second < 0) {
                min_s = 0;
            } else {
                min_s = current_second;
            }
        }
        
        for (int j = min_s; j < max_s; j++) {
            for (int i = min_f; i < max_f; i++) {
                if (i > 0 && j > 0 && i < w && j < h) {
                    mask[j*w+i] = 1;
                }
            }
        }
    }
    

    int depth = pt.z;
    
        //画像を8ピクセル間隔でスキャン
        for (int i = 0; i < w; i++){
            for (int j = 0; j < h; j++){
                //ピクセルのRGBの値を取得
                 //for( iter = pts.begin(); iter != pts.end(); iter++ ){
                    
                    // 2枚目の写真描写
                    int pt_of_hand = w * j + i;
                    if (mask[pt_of_hand] == 1) {
                        // 指の座標のmaskピクセルが白の場合
                        int valueR;
                        int valueG;
                        int valueB;
                        if (depth > threshold_of_depth) {
                            // ２枚目の画像を持ってくる処理
                            valueR = pixels_2[j*3 * w + i*3];
                            valueG = pixels_2[j*3 * w + i*3+1];
                            valueB = pixels_2[j*3 * w + i*3+2];
                        } else {
                            valueR = pixels_3[j*3 * w + i*3];
                            valueG = pixels_3[j*3 * w + i*3+1];
                            valueB = pixels_3[j*3 * w + i*3+2];
                        }
                        ofSetColor(valueR, valueG, valueB);
                        ofCircle(i+offset_x, j+offset_y, 1);
                    }
                    
                //}
                
            }
        }
    
    
    
    // 画像描写
    ofSetColor(255, 255, 255); //おまじない
    myImage.draw(offset_x,offset_y);
    
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

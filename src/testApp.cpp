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

int threshold_of_pts = 15;

int window_width;
int window_height;

time_t time_now;

//--------------------------------------------------------------
void testApp::setup(){
    
    time_now = time(NULL);

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);

	leap.open(); 

	/*
    l1.setPosition(200, 300, 50);
	l2.setPosition(-200, -200, 50);*/
    l1.setPosition(200, 300, 800);
    l2.setPosition(-200, -200, 800);
    
    //l2.setPosition(<#float px#>, <#float py#>, <#float pz#>)

    cam.setOrientation(ofPoint(-20, 0, 0));

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    //画面の基本設定
    ofBackground(0,0,0);
    ofEnableSmoothing();
    //画面の混色の設定を加算合成にする
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //画像データの読込み
    // サーバ画像参照
    /*
    string image_url_1 = "http://160.16.56.118/hd15/rslt/img/main.jpg"; // stringはダブルクォーテーションじゃないとだめ
    string image_url_2 = "http://160.16.56.118/hd15/rslt/img/1.jpg";
    string image_url_3 = "http://160.16.56.118/hd15/rslt/img/2.jpg";
    myImage.loadImage(image_url_1);
    myImage_2.loadImage(image_url_2);
    myImage_3.loadImage(image_url_3);
    */
    
    // ローカル画像参照
    myImage.loadImage("1.jpg");
    myImage_2.loadImage("2.jpg");
    myImage_3.loadImage("3.jpg");
    titleImage.loadImage("title.jpg");
    
    
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
    
    simpleHands = leap.getSimpleHands();
    
    if ( leap.isFrameNew() && simpleHands.size() ){
    
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
    
        for (int i = 0; i < simpleHands.size(); i++){
                int id = simpleHands[i].fingers[1].id;
                /*
                 * Get the position of the finger.
                 */
            pt = simpleHands[i].fingers[1].pos;
            pt.x += 400;
            pt.y = -pt.y;
        }
    }

	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new. 
	leap.markFrameAsOld();	
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //画像の幅と高さを所得
    int w = myImage.width;
    int h = myImage.height;
    
    int offset_x = (window_width - w)/2;
    int offset_y = (window_height - h)/2;
    
    
    // タイトルイメージ用
    int t_w = titleImage.width;
    int t_h = titleImage.height;
    
    int t_offset_x = (window_width - t_w)/2;
    int t_offset_y = (window_height - t_h)/2;
    
    ofSetColor(255, 255, 255);
    
    time_t now = time(NULL);
    int time_d = now - time_now;
    if (time_d < 5) {
        //titleImage.draw(t_offset_x, t_offset_y-150);
        titleImage.draw(0, 0, window_width, window_height);
    }
    
    
    //読み込んだ画像データを画面に描画

    //画像データのビットマップ情報を配列に格納
    unsigned char * pixels = myImage.getPixels();
    unsigned char * pixels_2 = myImage_2.getPixels();
    unsigned char * pixels_3 = myImage_3.getPixels();
    
    // Mask
    int width_of_mask = 960;
    int height_of_mask = 640;
    unsigned char mask[width_of_mask * height_of_mask];
    for (int i=0; i < (w * h) + 1; i++) {
        // すべてのピクセルを黒に初期化する
        mask[i] = 0;
    }
    
    // 指の座標を取得
    ofSetColor(0, 0, 0);
    ofCircle(pt.x+offset_x, pt.y+offset_y, 10);
    
    ofSetColor(255, 255, 255);
    
    
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
	
	ofDisableLighting();
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
    /*for(int i = 0; i < simpleHands.size(); i++){
        simpleHands[i].debugDraw();
    }*/

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

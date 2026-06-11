#include <gmock/gmock.h>

/* main 함수 google test gmock 사용 */
int main() {
	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();

	//CarFactory factory;
	//// 자동차 부품 생성
	//auto engine = factory.createEngine("V8");
	//auto tire = factory.createTire("18 inch");
	//auto body = factory.createBody("Red");
	//// 자동차 조립
	//Car car = factory.assembleCar("V8", "18 inch", "Red");
	//car.show(); // 자동차 부품 출력
	//return 1;
}
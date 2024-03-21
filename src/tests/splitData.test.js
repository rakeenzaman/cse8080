import { splitData } from "../public/script";

test('Splits string between semicolons', function () {

    let str = 'one;two;three';

	expect(splitData(str)).toStrictEqual(['one', 'two', 'three']);
});